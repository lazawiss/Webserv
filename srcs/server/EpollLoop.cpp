/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:47:38 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/11 14:43:28 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lexer/Lexer.hpp"
#include "../parser/Parser.hpp"
#include <sstream>

#include "CGIHandler.hpp"
#include "EpollLoop.hpp"
#include "ListenerManager.hpp"
#include "Server.hpp"
#include "RequestHandler.hpp"

/*
** ============================================================================
** The Rule of Three
** ============================================================================
*/

EpollLoop:: EpollLoop() : _header(), _content() {}

EpollLoop::EpollLoop( EpollLoop const & src ) :
    _clientToListener(src._clientToListener),
    _clientResponseBuffer(src._clientResponseBuffer),
    _header(src._header), _content(src._content) {}

EpollLoop::~EpollLoop() {}

EpollLoop & EpollLoop::operator=( EpollLoop const & other )
{
    if ( this != &other)
    {
        _clientToListener       = other._clientToListener;
        _clientResponseBuffer   = other._clientResponseBuffer;
        _header                 = other._header;
        _content                = other._content;
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
int EpollLoop::setnonblocking( int fd ) {
    
    int result;
    int flags;

    flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;

    flags |= O_NONBLOCK;

    result = fcntl(fd , F_SETFL , flags);
    return result;
}

static RequestState analyzeRequest( const std::string &acc, std::string &prereq)
{
    size_t headerEnd = acc.find("\r\n\r\n");
    if (headerEnd == std::string::npos)
        return REQ_INCOMPLETE;
    size_t bodyStart = headerEnd + 4;

    std::string te;
    if // (chunked defined by Transfer-encoding in header block)
    {
        std::string decoded;
        RequestState st = dechunkBody(acc.substr(bodyStart), decoded);
        if (st != REQ_READY)
            return st;  // still arriving, or malformed
        ready = rebuildWithContentLength(acc, headerEnd, decoded);
        return REQ_READY;
    }

// if (content length then, have we go everything)
    {
        size_t expected = (size_t)strtoul(cl.c_str(), NULL, 10);
        if (acc.size() - bodyStart < expected)
            return REQ_INCOMPLETE;
    }

//etiehr no body or is complet
    ready = acc;
    return REQ_READY;
}


void    EpollLoop::cleanupClient(int fd, int epollfd)
{
    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    _clientResponseBuffer.erase(fd);
    _clientToListener.erase(fd);
}
/**
** @brief Reads a request from a client fd and builds the response.
**
** Dispatches to CGI or static file handler. All fd closing happens
** here — never in sub-classes. Returns false on error (fd closed).
**
** @param fd        the client file descriptor
** @param listeners all active listener sockets
** @param config    the global server configuration
** @param epollfd   the epoll instance fd
** @param ev        reused epoll_event struct
** @return          true on success, false on error
**/
bool EpollLoop::do_read_fd(
    int fd, std::vector<ListenerManager*> const & listeners,
    const GlobalConfig & config, int epollfd, epoll_event & ev)
{

    char    buf[BUF_SIZE];

    ssize_t n_read = read(fd, buf, BUF_SIZE);
    if (n_read <= 0)
    {
        LOG_ERROR("Client closed connectio or read error");
        cleanupClient(fd, epollfd);
        return false;
    }
    // Like our repsonses, we create new entry in map; if not found, creates
    std::string &acc = _clientRequestBuffer[fd];
    acc.append(buf, n_read);
    if (acc.size() > (size_t)BUF_SIZE)
    {
        cleanupClient(fd, epollfd);
        return false;
    }

    std::string prereq;
    RequestState state = analyzeRequest(acc, prereq);
    if (state == REQ_INCOMPLETE)
        return true;
    if (state == REQ_BAD){
        cleanupClient(fd, epollfd);
        return false;
    }
    if (state == REQ_READY)
    {
        
    }
    
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
        return (close(fd), false);
    }

    // match the ServerConfig whose port matches this listener
    const ServerConfig *serverConfig = NULL;
    const std::vector<ServerConfig> &servers = config.getServers();
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
        _clientToListener.erase(fd);  
        return (close(fd), false);
    }

    RequestHandler requestHandler(request, *serverConfig);

    if (requestHandler.handleRequest(*listener) == false)
    {
        LOG_ERROR("handleRequest failed, sending 500");
        _clientResponseBuffer[fd] =
            "HTTP/1.1 500 Internal Server Error\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 0\r\n\r\n";
        ev.events = EPOLLOUT;

        ev.data.fd = fd;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
        
        return true;
    }
    if (requestHandler.getCGI())
    {
        CGI *cgi = new CGI(requestHandler, *listener, fd);
        if (!cgi->start())
        {
            delete cgi;
            _clientResponseBuffer[fd] =
              "HTTP/1.1 502 Bad Gateway\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 0\r\n\r\n";

            // send(fd, err.c_str(), err.size(), 0);
            // _clientToListener.erase(fd);
            // return (close(fd), false);
            ev.events = EPOLLOUT;

            ev.data.fd = fd;
            epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
            
            return true;
        }

        // stdin pipe: WE write the body into it -> watch for EPOLLOUT
        ev.events = EPOLLOUT;
        ev.data.fd = cgi->getStdinFd();
        epoll_ctl(epollfd, EPOLL_CTL_ADD, cgi->getStdinFd(), &ev);
        _fdToCGI[cgi->getStdinFd()] = cgi;

        // stdout pipe: WE read the script output -> watch for EPOLLIN
        ev.events = EPOLLIN;
        ev.data.fd = cgi->getStdoutFd();
        epoll_ctl(epollfd, EPOLL_CTL_ADD, cgi->getStdoutFd(), &ev);
        _fdToCGI[cgi->getStdoutFd()] = cgi;

        // client fd stays OPEN and untouched: the response is sent later,
        // when the stdout pipe hits EOF (see readingSocket)
        return true;
    }

    std::ostringstream dbg;
    dbg << "Response header built for fd=" << fd;
    LOG_DEBUG(dbg.str());

    _clientResponseBuffer[fd] += std::string(requestHandler.getHeader());
    _clientResponseBuffer[fd] += std::string(
        requestHandler.getBuffer().c_str(),
        requestHandler.getNReadIndex());

    
    ev.events = EPOLLOUT;

    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    
    return true;

}

bool EpollLoop::do_write_fd( int fd, int epollfd, epoll_event &ev ) {
     
    std::string & response = _clientResponseBuffer[fd];
    if (response.empty())
        return false;

    ssize_t headerSent = send(fd, response.c_str(), response.size(), 0);
    if (headerSent == -1)
    {
        std::ostringstream oss;
        oss << "Send error on fd: " << fd;
        LOG_ERROR(oss.str());
    
        close(fd);
        _clientResponseBuffer.erase(fd);
        _clientToListener.erase(fd);
 
        return false;
    }

    if (headerSent < static_cast<ssize_t>(response.size()))
    {
        response = response.substr(headerSent);
        ev.events = EPOLLOUT;

        ev.data.fd = fd;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    
        return true;
    }

    LOG_INFO("Message completely sent.");

    epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    _clientResponseBuffer.erase(fd);
    _clientToListener.erase(fd);

    return true;

}

bool EpollLoop::readingSocket(
    std::vector<ListenerManager*> const & listeners,
    const GlobalConfig & config)
{

    struct epoll_event ev, events[MAX_EVENTS];

    // create epoll instance in the kernel, returns epollfd
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
        if (epoll_ctl(epollfd, EPOLL_CTL_ADD,
                listeners[i]->getSockfd(), &ev) == -1)
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
        for (int n = 0; n < nfds; ++n)
        {
            struct sockaddr_storage peer_addr;
            socklen_t               peer_addr_len = sizeof(peer_addr);
            
            int listenerSockfd = -1;
            for (size_t i = 0; i < listeners.size(); i++)
            {
                if (events[n].data.fd == listeners[i]->getSockfd())
                {
                    listenerSockfd = listeners[i]->getSockfd();
                    break;
                }
            }

            if (listenerSockfd != -1) {
                // create a dedicated fd for this client
                // peer_addr holds the client IP address
                int clientfd = accept(listenerSockfd,
                    (struct sockaddr *) &peer_addr, &peer_addr_len);
                if (clientfd == -1)
                {
                    LOG_ERROR("accept() failed - "
                        + std::string(strerror(errno)));
                    break;
                }
                // set client fd non-blocking: read() returns EAGAIN if no data
                // yet required with EPOLLET to avoid blocking the program
                if (setnonblocking(clientfd) < 0)
                {
                    LOG_ERROR("setnonblocking() failed - "
                        + std::string(strerror(errno)));
                    close(clientfd);
                    break;
                }
                // add client fd to the kernel epoll table
                // EPOLLIN only because new client have nothing to write yet
                ev.events = EPOLLIN;

                ev.data.fd = clientfd;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev) == -1)
                {
                    LOG_ERROR("epoll_ctl(2) failed - "
                        + std::string(strerror(errno)));
                    close(clientfd);
                    break;
                }
                _clientToListener[clientfd] = listenerSockfd;
            
            } else {
                
                std::map<int, CGI*>::iterator it =
                    _fdToCGI.find(events[n].data.fd);
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
                    
                    } else {
                        // collecting the script's output stdout pipe until EOF
                        if (cgi->onReadable())
                        {
                            // EOF: script finished -> reap child, respond
                            epoll_ctl(epollfd, EPOLL_CTL_DEL, activeFd, NULL);
                            _fdToCGI.erase(it);
                            cgi->closeStdout();

                            int clientFd = cgi->getClientFd();
                            _clientResponseBuffer[clientFd]
                                += cgi->buildResponse();
                            ev.events  = EPOLLOUT;
                            ev.data.fd = clientFd;
                            epoll_ctl(epollfd, EPOLL_CTL_MOD,
                                clientFd, &ev);
                            delete cgi;
                        }
                    }
                
                } else {
                    
                    if ( events[n].events & EPOLLIN ) {  

                        if (!do_read_fd(events[n].data.fd,
                                listeners, config, epollfd, ev))
                            break;
                    
                    } else if ( events[n].events & EPOLLOUT ) {

                        if (!do_write_fd(events[n].data.fd,
                                epollfd, ev))
                            break;
                    }
                }
            }
        }
    }

    close(epollfd);

    return true;
}
