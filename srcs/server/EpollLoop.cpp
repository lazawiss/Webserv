/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:47:38 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/20 16:47:09 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "EpollLoop.hpp"
#include "Server.hpp"
#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
#include "ResponseSender.hpp"
#include "../parser/Parser.hpp"
#include "RequestHandler.hpp"


/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

EpollLoop:: EpollLoop() {}

EpollLoop::EpollLoop( EpollLoop const & src ){
    
    *this = src;
}

EpollLoop::~EpollLoop() {}

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

/**
** @brief Sets a file descriptor to non-blocking mode.
**
** Uses fcntl() to read the current flags of the fd and adds O_NONBLOCK.
** After this call, read() on this fd returns immediately with EAGAIN 
** instead of blocking if no data is available.
**
** @param fd  the file descriptor to modify
** @return    result of fcntl(F_SETFL), -1 on error
**/
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
// handle fds : no closing fds in other classes only in EpollLoop 
// TO ENSURE NO HANGING FDS : if boolean == false > error caught fd closed in EPollLoop
// then throw in Server >> quit program
bool    EpollLoop::do_use_fd(  int fd, ListenerManager const & listen ){
    
    char    buf[BUF_SIZE];

    ssize_t n_read = read(fd, buf, BUF_SIZE);           // read HTTP requests
    
    if (n_read == -1)
    {
        if (errno == EAGAIN || errno == EWOULDBLOCK)    // FOR PORTABILITY
            return true;
        else
        {
            
            std::cerr << " Do_use_fd : Error reading from fd "<< fd << strerror(errno) << std::endl;
            close(fd);
            return false;
        }
    }
    else if (n_read == 0)
    {
        
        std::cout << "Client closed connection : "<< fd << std::endl;
        close(fd);
        return false;
    }
    
    std::string request = std::string(buf, n_read);
    
    // Parse request
    RequestHandler requestHandler(request);
    
    if (requestHandler.handleRequest(listen) == false){
        std::cerr << "Reading of html file failed: " << strerror(errno) << std::endl;
        close(fd);
        return false;
    }

    // // should fork() here : ONLY FORK() FOR CGI
    
    // // send response  
    std::cout << "header:" << requestHandler.getHeader() << std::endl;
    std::string header = std::string (requestHandler.getHeader());
    std::string content = std::string(requestHandler.getBuffer().c_str(), requestHandler.getNReadIndex());
    
    ResponseSender  responseSender( header, content, fd);
    
    
    if (responseSender.sendResponse() == false){
            std::cerr << "Error sending response: " << strerror(errno) << std::endl;
            close(fd);
            return false;
        } 
        
    // if (send(fd, header.c_str(), header.size(), 0) < 0){
    //     std::cerr << "Error sending response: " << strerror(errno) << std::endl;
    //     close(fd);
    //     return false;
    // }
    
    // ssize_t totalSent = 0;
        
    // while (totalSent < requestHandler.getNReadIndex()){
        
    //     // ssize_t sent = send(fd, content.c_str(), content.size(), 0) < 0;
    //     ssize_t sent = send(fd, content.c_str() + totalSent, content.size() - totalSent, 0) < 0;
        
        
    //     if (sent == -1){
    //         std::cerr << "Error sending response: " << strerror(errno) << std::endl;
    //         close(fd);
    //         return false;
    //     }
            
    //     totalSent += sent;
    // }
    
    close(fd);
    return true;
}

bool    EpollLoop::readingSocket( ListenerManager const & listen ){

    // ev     : reused form for each epoll_ctl() call to register a fd
    // events : filled by epoll_wait() with the currently active fds
    struct epoll_event ev, events[MAX_EVENTS];

    // create epoll instance in the kernel, returns epollfd (e.g. fd 4)
    int epollfd = epoll_create(sizeof ev);
    if (epollfd == -1)
    {
        LOG_ERROR("epoll_create() failed - " + std::string(strerror(errno)));
        return false;
    }

    // register server fd (_socketfd) in the kernel epoll table
    // EPOLLIN = notify when a client wants to connect
    ev.events = EPOLLIN;
    ev.data.fd = listen.getSockfd();
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen.getSockfd(), &ev) == -1)
    {
        LOG_ERROR("epoll_ctl(1) failed - " + std::string(strerror(errno)));
        close(epollfd);
        return false;
    }
 
// main loop : runs until Ctrl+C signal (_quit = 1)
    while (Server::_quit != 1){
        
        // sleep until a fd becomes active (max 100ms)
        // returns nfds = number of active fds, 0 if timeout, -1 if error
        int nfds = epoll_wait(epollfd, events, MAX_EVENTS, 100);
        if (nfds == -1)
        {
            // EINTR = signal received (Ctl+C) → go back to while to check _quit
            if (errno == EINTR)
                continue;
            LOG_ERROR("epoll_wait() failed — " + std::string(strerror(errno)));
            close(epollfd);
            break;
        } 
        
        // process each active fd returned by epoll_wait
        for (int n = 0; n < nfds; ++n){
            
            struct sockaddr_storage peer_addr;
            socklen_t               peer_addr_len = sizeof(peer_addr);
            
            // active fd == server fd → new client trying to connect
            if (events[n].data.fd == listen.getSockfd())
            {
                // create a dedicated fd for this client (e.g. fd 5)
                // peer_addr holds the client IP address
                int clientfd = accept(listen.getSockfd(),
                    (struct sockaddr *) &peer_addr, &peer_addr_len);
                if (clientfd == -1)
                {
                    LOG_ERROR("accept() failed - " + std::string(strerror(errno)));
                    break;
                }
                // set client fd non-blocking: read() returns EAGAIN if no data
                // yet required with EPOLLET to avoid blocking the program
                if (setnonblocking(clientfd) < 0)
                {
                    LOG_ERROR("setnonblocking() failed - " + std::string(strerror(errno)));
                    close(clientfd);
                    break;
                }
                // add client fd to the kernel epoll table
                // EPOLLET = notify only once when data arrives
                ev.events = EPOLLIN | EPOLLET;
                ev.data.fd = clientfd;
                if (epoll_ctl(epollfd,EPOLL_CTL_ADD,clientfd, &ev) == -1)
                {
                    LOG_ERROR("epoll_ctl(2) failed - " + std::string(strerror(errno)));
                    close(clientfd);
                    break;
                }
            }
            // active fd == client fd → client is sending its HTTP request
            else
            {
                if (do_use_fd(events[n].data.fd, listen) == false)
                    break;
            }
        }
    }

    close(epollfd);

    return true;
}
