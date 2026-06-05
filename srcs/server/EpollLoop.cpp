/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:47:38 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/05 15:40:15 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "EpollLoop.hpp"
#include "Server.hpp"
#include "ListenerManager.hpp"
#include "ResponseWriter.hpp"



/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

EpollLoop:: EpollLoop(){

}

EpollLoop::EpollLoop( EpollLoop const & src ){
    
    *this = src;
}

EpollLoop::~EpollLoop(){
    
}

EpollLoop & EpollLoop::operator=( EpollLoop const & other ){
    
    if ( this != &other)
        *this = other;

    return *this;
}


/*
** ============================================================================
** Main Event Loop
** ============================================================================
*/

//set flags for fcntl():
int EpollLoop::setnonblocking( int fd ){
    
    int result;
    int flags;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;

    flags |= O_NONBLOCK;

    result = fcntl(fd , F_SETFL , flags);
    return result;
}

// open dialogue with client:
// read request
// parse request
// answer : send response
// CGI >> fork 
bool    EpollLoop::do_use_fd(  int fd ){
    
    char    buf[BUF_SIZE];

    // read request :
    ssize_t n_read = read(fd, buf, BUF_SIZE); // read HTTP requests
    
    std::cout << "n_read:" << n_read << std::endl;
    if (n_read == -1){
        if (errno == EAGAIN || errno == EWOULDBLOCK) // FOR PORTABILITY
            return true;
        else{
            
            std::cerr << " Do_use_fd : Error reading from fd "<< fd << strerror(errno) << std::endl;
            close(fd);
            return false;
        }
    }
    else if (n_read == 0){
        
        std::cout << "Client closed connection : "<< fd << std::endl;
        close(fd);
        return false;
    }
    
    std::string request = std::string(buf, n_read);
    std::cout << "Request on fd " << fd << ": " << request << std::endl;
    
    // int s = getsockname(fd, (struct sockaddr *) &peer_addr, &peer_addr_len);
    // if (fd == 0)
    // std::cout << "Received " << static_cast<long>(n_read) << "bytes from " << peer_addr.ss_family << ":" << std::endl;
    // else
    // std::cout << "getnameinfo: " << gai_strerror(s) << std::endl;
    
    // Parse request
    HTTPParser HTTPparser(request);
    
    if (HTTPparser.findMethods() == false){
         std::cerr << "Reading of html file failed: " << strerror(errno) << std::endl;
        close(fd);
        return false;
    }
    // if (request.find("GET / HTTP/1.1") != std::string::npos){ // same as EOF
        
    //     char        buffer[BUF_SIZE];

    //     int indexfd = open("data/html/index.html", O_RDONLY);
    //     ssize_t n_read_index = read(indexfd, buffer, BUF_SIZE);
    //     close(indexfd);
    //     std::cout << "n_read_index:" << n_read_index << std::endl;
    //     if (n_read_index == -1){
    //         std::cerr << "Reading of html file failed: " << strerror(errno) << std::endl;
    //         close(fd);
    //         return false;
    //     }
    //     std::string response = std::string(buffer,n_read_index);
    //     if (send(fd, response.c_str(), response.size(), 0) < 0){
            
    //         std::cerr << "Error sending response: " << strerror(errno) << std::endl;
    //         // close(_fd);
    //         return false;
    //     }
        
    // }
    // should fork() here : ONLY FORK() FOR CGI
        
    // send response  
        // std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>Hello from C++!</body></html>";
    std::string response = std::string(HTTPparser.getBuffer().c_str(), HTTPparser.getNReadIndex());

    ResponseWriter  responseWriter( response, fd);

    if (responseWriter.sendResponse() == false){
        std::cerr << "Error sending response: " << strerror(errno) << std::endl;
        close(fd);
        return false;
    }
    
    close(fd);
    return true;
}

// main loop event : 
// accept : create dynamically a new connected socket for each new client, return a new int fd
// 
// getsockname : returns current socket address
bool    EpollLoop::readingSocket( ListenerManager const & listen ){
    

    std::cout << "_quit:" << Server::_quit << std::endl;

    struct epoll_event ev, events[MAX_EVENTS];

    int epollfd = epoll_create(sizeof ev); //instantiate epoll 
    if (epollfd == -1){
        std::cerr << "epollfd:" << epollfd << " " << strerror(errno) << std::endl;
        return false;
    }

    ev.events = EPOLLIN;
    ev.data.fd = listen.getSockfd();
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen.getSockfd(), &ev) == -1){
        std::cerr << "Call to epoll_ctl(1) failed: "<< strerror(errno) << std::endl;
        close(epollfd);
        return false;
    }
 
// epoll loop :
    while(Server::_quit != 1){
        
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, 100);
        
        // std::cout << "nfds:" << nfds << std::endl;
        if (nfds == -1){
            if (errno == EINTR)
                continue;
            std::cerr << "Error epoll_wait " << nfds << ": "<< strerror(errno) << std::endl;
            close(epollfd);
            break;
        } 
        for ( int n = 0; n < nfds; ++n){
            
            struct sockaddr_storage peer_addr;
            socklen_t               peer_addr_len = sizeof(peer_addr);
            
            if (events[n].data.fd == listen.getSockfd()){
                
                int clientfd = accept(listen.getSockfd(), (struct sockaddr *) &peer_addr, &peer_addr_len );
                std::cout << "clientfd:" << clientfd << std::endl;
                if (clientfd == -1){
                    std::cerr << strerror(errno) << std::endl;
                    break;
                }
                if (setnonblocking(clientfd) < 0){
                    std::cerr << strerror(errno) << std::endl;
                    close(clientfd);
                    break;
                }
                ev.events = EPOLLIN | EPOLLET; // EPOLLET>>EAGAIN
                ev.data.fd = clientfd;
                if (epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd, &ev) == -1){
                    std::cerr << "Call to epoll_ctl(2) failed: "<< strerror(errno) << std::endl;
                    close(clientfd);
                    break;
                }
            }
            else{
                
                if(do_use_fd(events[n].data.fd) == false)
                    break;
            }
                    
        }

    }
//basic server loop:
    // while(_quit != 1){
        
    //     struct sockaddr_storage peer_addr;
    //     char                    buf[BUF_SIZE];

        
    //     socklen_t peer_addr_len = sizeof(peer_addr);

    //     int clientfd = accept(_sockfd, (struct sockaddr *) &peer_addr, &peer_addr_len );
    //     std::cout << "clientfd:" << clientfd << std::endl;

    //     // should fork() here : ONLY FORK() FOR CGI
    //     ssize_t n_read = read(clientfd, buf, BUF_SIZE); // read HTTP requests

    //     std::cout << "n_read:" << n_read << std::endl;
    //     if (n_read == -1){
    //         std::cerr << strerror(errno) << std::endl;
    //         close(clientfd);
    //         continue;
    //     }
    //     std::string request = std::string(buf, n_read);
    //     std::cout << "Request: " << request << std::endl;
        
    //     int s = getsockname(clientfd, (struct sockaddr *) &peer_addr, &peer_addr_len);
    //     if (clientfd == 0)
    //         std::cout << "Received " << static_cast<long>(n_read) << "bytes from " << peer_addr.ss_family << ":" << std::endl;
    //     else
    //         std::cout << "getnameinfo: " << gai_strerror(s) << std::endl;
            
    //     // Parse request
    //     if (request.find("GET / HTTP/1.1") != std::string::npos){ // same as EOF
            
    //         char    buffer[BUF_SIZE];

    //         // std::ifstream file("data/html/index.html".c_str());
    //         int indexfd = open("data/html/index.html", O_RDONLY);
    //         ssize_t n_read_index = read(indexfd, buffer, BUF_SIZE);
    //         std::cout << "n_read_index:" << n_read_index << std::endl;
    //         if (n_read_index == -1){
    //             std::cerr << strerror(errno) << std::endl;
    //             close(indexfd);
    //             continue;
    //         }

    //         // std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<html><body>Hello from C++!</body></html>";
    //         std::string response = std::string(buffer, n_read_index);

    //         if (send(clientfd, response.c_str(), response.size(), 0))
    //             std::cout << "Error sending response" << std::endl;
    //     } 

    //     close(clientfd);

    // }

    close(epollfd);
    return true;
}
