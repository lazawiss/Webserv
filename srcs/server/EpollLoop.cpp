/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:47:38 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/26 13:59:52 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "EpollLoop.hpp"
#include "Server.hpp"
#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
#include "../parser/Parser.hpp"
#include "RequestHandler.hpp"
#include "CGIHandler.hpp"
#include <sstream>    // std::ostringstream, to stringify the decoded body length
#include <strings.h>  // strncasecmp, for case-insensitive header-name matching



/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

EpollLoop:: EpollLoop() : _header(), _content(){}

EpollLoop::EpollLoop( EpollLoop const & src ) : _clientToListener(src._clientToListener), _clientResponseBuffer(src._clientResponseBuffer), _clientRequestBuffer(src._clientRequestBuffer), _header(src._header), _content(src._content){

}

EpollLoop::~EpollLoop() {}

EpollLoop & EpollLoop::operator=( EpollLoop const & other ){
    
    if ( this != &other)
    {
        _clientToListener = other._clientToListener;
        _clientResponseBuffer = other._clientResponseBuffer;
        _clientRequestBuffer = other._clientRequestBuffer;
        _header = other._header;
        _content = other._content;
    }

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


enum RequestState {
    REQ_INCOMPLETE, // valid but more need
    REQ_READY,
    REQ_BAD  // 400 rep?
};

static RequestState analyzeRequest(const std::string &acc, std::string &ready)
{
    size_t headerEnd = acc.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return REQ_INCOMPLETE;
    // size_t bodyStart = headerEnd + 4;

    // std::string te;
//     if // (chunked defined by Transfer-encoding in header block)
//     {
//         std::string decoded;
//         RequestState st = dechunkBody(acc.substr(bodyStart), decoded);
//         if (st != REQ_READY)
//             return st;  // still arriving, or malformed
//         ready = rebuildWithContentLength(acc, headerEnd, decoded);
//         return REQ_READY;
//     }

// // if (content length then, have we go everything)
//     {
//         size_t expected = (size_t)strtoul(cl.c_str(), NULL, 10);
//         if (acc.size() - bodyStart < expected)
//             return REQ_INCOMPLETE;
//     }

// //etiehr no body or is complet
    ready = acc;
    return REQ_READY;
}

void    EpollLoop::cleanupClient( int fd, int epollfd ){

    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    _clientRequestBuffer.erase(fd);
    // _clientResponseBuffer.erase(fd);
    // _clientToListener.erase(fd);
}

// open dialogue with client:
// read request
// parse request
// answer : send response
// CGI >> fork
// handle fds : no closing fds in other classes only in EpollLoop
// TO ENSURE NO HANGING FDS : if boolean == false > error caught fd closed in EPollLoop
// then throw in Server >> quit program
bool    EpollLoop::do_read_fd( int fd, std::vector<ListenerManager*> const & listeners, const GlobalConfig &config, int epollfd, epoll_event &ev){

    std::cout << "[global] root: " << config.getRoot() << std::endl;
    const std::vector<ServerConfig> &servers = config.getServers();
    for (size_t i = 0; i < servers.size(); i++)
    {
        std::cout << "[server " << i << "] root: " << servers[i].getRoot() << std::endl;
        const std::vector<LocationConfig> &locations = servers[i].getLocations();
        for (size_t j = 0; j < locations.size(); j++)
            std::cout << "[server " << i << "][location " << j << "] root: " << locations[j].getRoot() << std::endl;
    }

    //can we read chunk?
    char    buf[BUF_SIZE];
    ssize_t n_read = read(fd, buf, BUF_SIZE);

    if (n_read <= 0)
    {
        LOG_ERROR("Client closed connection or read error");
        cleanupClient(fd, epollfd);
        return false;
    }

    std::string &acc = _clientRequestBuffer[fd]; // like response - new entry in map creates if not found
    acc.append(buf, n_read);

    if (acc.size() > (size_t)BUF_SIZE) // check but maybe then buffer before should be smoller?
    {
        // gonna be 413 and then send () 0 check request handler here
        cleanupClient(fd, epollfd);
        return false;
    }
    std::string ready;
    RequestState state = analyzeRequest(acc, ready);
    if (state == REQ_INCOMPLETE)
        return true; // to wait for more bytes
    if (state == REQ_BAD)
    {
        // error 400 and then send from request hanlder her
        cleanupClient(fd, epollfd);
        return false;
    }

    _clientRequestBuffer.erase(fd); // bc noramlly. I sent all request, so need to clean this one up 

    // std::cout << " RAW REQUEST \n" << normalized << "\nEND OF REQUEST" << std::endl;

    // find which listener accepted this client
    int listenerSockfd = _clientToListener[fd];
    const ListenerManager *listener = NULL;
    for (size_t i = 0; i < listeners.size(); i++)
    {
        if (listeners[i]->getSockfd() == listenerSockfd)
        {
            listener = listeners[i];
            break;
        }
    }
    if (listener == NULL)
    {
        LOG_ERROR("No listener found for fd " + std::string(strerror(errno)));
        cleanupClient(fd, epollfd);
        return false;
    }

    // match the ServerConfig whose port matches this listener
    const ServerConfig *serverConfig = NULL;
    // const std::vector<ServerConfig> &servers = config.getServers();
    for (size_t i = 0; i < servers.size(); i++)
    {
        const std::string &listenVal = servers[i].getListen();
        size_t colon = listenVal.find(':');
        std::string port = listenVal.substr(colon + 1);
        if (port == listener->getService())
        {
            serverConfig = &servers[i];
            break;
        }
    }
    if (serverConfig == NULL)
    {
        LOG_ERROR("No ServerConfig found for port " + listener->getService());
        cleanupClient(fd, epollfd);
        return false;
    }

    RequestHandler requestHandler(ready, *serverConfig);

    if (requestHandler.handleRequest(*listener) == false)
    {
        std::cerr << "Reading of html file failed: " << strerror(errno) << std::endl;
        return (close(fd), false);
    }
    if (requestHandler.getCGI())
    {
        CGI *cgi = new CGI(requestHandler, *listener, fd);
        if (!cgi->start())
        {
            delete cgi;
            // error 500 needed - request handler here
            _clientToListener.erase(fd);
            return (close(fd), false);
        }
        ev.events = EPOLLOUT;
        ev.data.fd = cgi->getStdinFd();
        epoll_ctl(epollfd, EPOLL_CTL_ADD, cgi->getStdinFd(), &ev);
        _fdToCGI[cgi->getStdinFd()] = cgi;

        ev.events = EPOLLIN;
        ev.data.fd = cgi->getStdoutFd();
        epoll_ctl(epollfd, EPOLL_CTL_ADD, cgi->getStdoutFd(), &ev);
        _fdToCGI[cgi->getStdoutFd()] = cgi;

        // client fd stays OPEN and untouched: the response is sent later,
        // when the stdout pipe hits EOF (see readingSocket)
        return true;
    }

    std::cout << "header:" << requestHandler.getHeader() << std::endl;
    //_header = std::string(requestHandler.getHeader());
    //_content = std::string(requestHandler.getBuffer().c_str(), requestHandler.getNReadIndex());
    _clientResponseBuffer[fd] += std::string(requestHandler.getHeader());
    _clientResponseBuffer[fd] += std::string(requestHandler.getBuffer().c_str(), requestHandler.getNReadIndex());

    
    ev.events = EPOLLOUT; // only write

    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    
    return true;

}

bool    EpollLoop::do_write_fd( int fd, int epollfd, epoll_event &ev ){
     
    std::string & response = _clientResponseBuffer[fd];
    ssize_t headerSent = send(fd, response.c_str(), response.size(), 0);

    if (response.empty()){
        return false;
    }

    if (headerSent == -1){
        
        LOG_ERROR("Send error on fd: " + fd);
        close(fd);
        _clientResponseBuffer.erase(fd);
        _clientToListener.erase(fd);  
        return false;
    }
    // if (headerSent == 0){
        
    //     LOG_ERROR("Connection closed during send");
    //     close(fd);
    //     _clientResponseBuffer.erase(fd);
    //     _clientToListener.erase(fd);  
    //     return false;
    // }
    if (headerSent < static_cast<ssize_t>(response.size())){
        
        //_clientResponseBuffer[fd] = response.substr( headerSent );
        response = response.substr( headerSent );
        ev.events = EPOLLOUT;

        ev.data.fd = fd;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
        return true;
    }

    LOG_INFO("Message completly sent.");

    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    _clientResponseBuffer.erase(fd);
    _clientToListener.erase(fd);
    return true;

}

bool EpollLoop::readingSocket( std::vector<ListenerManager*> const & listeners, const GlobalConfig &config ){

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

    // register all listener fds in the kernel epoll table
    // EPOLLIN = notify when a client wants to connect
    ev.events = EPOLLIN;
    for (size_t i = 0; i < listeners.size(); i++)
    {
        ev.data.fd = listeners[i]->getSockfd();
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listeners[i]->getSockfd(), &ev) == -1)
        {
            LOG_ERROR("epoll_ctl(1) failed - " + std::string(strerror(errno)));
            return close(epollfd), false;
        }
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
            
            // check if active fd is one of the listener fds → new client connecting
            int listenerSockfd = -1;
            for (size_t i = 0; i < listeners.size(); i++)
            {
                if (events[n].data.fd == listeners[i]->getSockfd())
                {
                    listenerSockfd = listeners[i]->getSockfd();
                    break;
                }
            }

            if (listenerSockfd != -1)
            {
                // create a dedicated fd for this client (e.g. fd 5)
                // peer_addr holds the client IP address
                int clientfd = accept(listenerSockfd,
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
                ev.events = EPOLLIN; //EPOLLIN only because new client have nothing to write yet

                ev.data.fd = clientfd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev) == -1)
                {
                    LOG_ERROR("epoll_ctl(2) failed - " + std::string(strerror(errno)));
                    close(clientfd);
                    break;
                }
                _clientToListener[clientfd] = listenerSockfd;
            }
            // active fd == client fd → client is sending its HTTP request
            else
            {
                
                std::map<int, CGI*>::iterator it = _fdToCGI.find(events[n].data.fd);
                if (it != _fdToCGI.end())
                {
                    CGI *cgi = it->second;
                    int activeFd = events[n].data.fd;
                    // event only carries raw fd, not why you registered
                    if (activeFd == cgi->getStdinFd())
                    {
                        if (cgi->onWritable())
                        {
                            epoll_ctl(epollfd, EPOLL_CTL_DEL, activeFd, NULL);
                            _fdToCGI.erase(it);
                            cgi->closeStdin();
                        }
                    }
                    else
                    {
                        // collecting the script's output stdout pipe until EOF
                        if (cgi->onReadable())
                        {
                            // EOF: script finished -> reap child, respond
                            epoll_ctl(epollfd, EPOLL_CTL_DEL, activeFd, NULL);
                            _fdToCGI.erase(it);
                            cgi->closeStdout();

                            int clientFd = cgi->getClientFd(); 
                            _clientResponseBuffer[clientFd] += cgi->buildResponse();
                            ev.events = EPOLLOUT;
                            ev.data.fd = cgi->getClientFd();
                            epoll_ctl(epollfd, EPOLL_CTL_MOD, cgi->getClientFd(), &ev);
                            delete cgi;
                        }
                    }
                }
                else{
                    
                    if ( events[n].events & EPOLLIN ){
                        
                        if (do_read_fd(events[n].data.fd, listeners, config, epollfd, ev) == false)
                            break;
                    }
                    else if ( events[n].events & EPOLLOUT ){
                        
                        if (do_write_fd(events[n].data.fd, epollfd, ev) == false)
                            break;
                    }
                }
            }
        }
    }

    close(epollfd);

    return true;
}
