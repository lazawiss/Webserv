/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollLoop.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:47:38 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/26 20:58:35 by lzannis          ###   ########.fr       */
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

static bool getHeaderValue(
    const std::string &headerBlock, const std::string &name, std::string &out)
{
    size_t i = headerBlock.find("\r\n");
    if (i == std::string::npos)
        return false;
    i += 2;

    if (i < headerBlock.size() && (headerBlock[i] == ' ' || headerBlock[i] == '\t'))
        return LOG_ERROR("{PARTIAL REQUESTS} : Leading whitespace after request-line"), false;

    while (i < headerBlock.size())
    {
        size_t end = headerBlock.find("\r\n", i);
        if (end == std::string::npos)
            end = headerBlock.size();

        size_t colon = headerBlock.find(':', i);
        if (colon != std::string::npos && colon < end)
        {
            std::string key = headerBlock.substr(i, colon - i);
            if (key == name)
            {
                size_t valLineStart = colon + 1;
                while (valLineStart < end && (headerBlock[valLineStart] == ' ' || headerBlock[valLineStart] == '\t'))
                    valLineStart++;

                size_t valLineEnd = end;
                while (valLineEnd > valLineStart && (headerBlock[valLineEnd - 1] == ' ' || headerBlock[valLineEnd - 1] == '\t'))
                    valLineEnd--;

                out = headerBlock.substr(valLineStart, valLineEnd - valLineStart);
                return true;
            }
        }
        if (end == headerBlock.size())
            break;
        i = end + 2;
    }
    return false;
}

static std::string rebuildWithContentLength(
    const std::string &acc, size_t headerEnd, const std::string &decoded)
{
    const std::string headerBlock = acc.substr(0, headerEnd);
    std::string out;

    size_t i = 0;
    bool firstLine = true;
    while (i <= headerBlock.size())
    {
        size_t end = headerBlock.find("\r\n", i);
        if (end == std::string::npos)
            end = headerBlock.size();
        std::string line = headerBlock.substr(i, end - i);

        bool drop = false;
        if (!firstLine)
        {
            size_t colon = line.find(':');
            std::string key = (colon == std::string::npos)? line : line.substr(0, colon);
            if (key == "Transfer-Encoding" || key == "Content-Length")
                drop = true;
        }
        if (!drop)
        {
            out += line;
            out += "\r\n";
        }
        firstLine = false;
        if (end == headerBlock.size())
            break;
        i = end + 2; // next line
    }

    std::ostringstream cl;
    cl << "Content-Length: " << decoded.size() << "\r\n";
    out += cl.str();
    out += "\r\n";
    out += decoded;
    std::cout << " Return 'out' var" << out << std::endl;
    return out;
}


static RequestState dechunkBody( const std::string &body, std::string &decoded )
{
    size_t pos = 0;
    decoded.clear();
    std::cout << "FULL BODY :" << std::endl;
    while (true)
    {
        size_t lineEnd = body.find("\r\n", pos);
        if (lineEnd == std::string::npos)
            return REQ_INCOMPLETE; // line not complete

        std::string sizeStr = body.substr(pos, lineEnd - pos);
        size_t semi = sizeStr.find(';');
        if (semi != std::string::npos)
            sizeStr = sizeStr.substr(0, semi);
        if (sizeStr.empty())
            return REQ_BAD;

        size_t chunkSize = 0;


        std::stringstream ss;
        ss << sizeStr; 
        size_t num = 0;
        ss >> std::hex >> num;
        std::cout << "what is num : " << num << std::endl;
        chunkSize = num; 
        if (decoded.size() + chunkSize > BUF_SIZE)
            return REQ_BAD_413;
        

        std::cout << "Real chunk size in non hex: " << chunkSize << std::endl;

        size_t dataStart = lineEnd + 2; // +/r/n
        
        if (chunkSize == 0)
        {
            // final chunk require the \r\n
            if (body.size() < dataStart + 2)
                return REQ_INCOMPLETE;
            if (body.compare(dataStart, 2, "\r\n") != 0)
                return REQ_BAD;
            std::cout << "Decoded so full body done here: " << decoded << std::endl;
            return REQ_READY;
        }
        
        if (body.size() < dataStart + chunkSize + 2)
            return REQ_INCOMPLETE;
        if (body.compare(dataStart + chunkSize, 2, "\r\n") != 0)
            return REQ_BAD;

        decoded.append(body, dataStart, chunkSize);
        pos = dataStart + chunkSize + 2;
    }
}

static RequestState analyzeRequest( const std::string &acc, std::string &request)
{
    size_t headerEnd = acc.find("\r\n\r\n");
    if (headerEnd == std::string::npos)     
        return REQ_INCOMPLETE;
    size_t bodyStart = headerEnd + 4;
    const std::string headerBlock = acc.substr(0, headerEnd);

    //BODY PART:
    // chunking here
    std::string te;
    if (getHeaderValue(headerBlock, "Transfer-Encoding", te)
        && HTTPParser::containsCaseInsensitive(te, "chunked"))
    {
        std::string decoded;
        RequestState st = dechunkBody(acc.substr(bodyStart), decoded);
        std::cout << "ENUM request \n" << st << std::endl;
        if (st != REQ_READY)
            return st; 
        request = rebuildWithContentLength(acc, headerEnd, decoded);
        return REQ_READY;
    }
    //content length given 
    std::string cl;
    if (getHeaderValue(headerBlock, "Content-Length", cl))
    {
        for (size_t k = 0; k < cl.size(); k++)
            if (!std::isdigit((unsigned char)cl[k]))
                return REQ_BAD_411;
        size_t expected = (size_t)strtoul(cl.c_str(), NULL, 10);
        if (expected > BUF_SIZE)
            return REQ_BAD_413;
        if (acc.size() - bodyStart < expected)
            return REQ_INCOMPLETE; 
        request = acc.substr(0, bodyStart + expected);
        return REQ_READY;
    }
    // if no body request ends at the header term
    request = acc.substr(0, bodyStart);
    return REQ_READY;
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
    if (n_read == 0)
    {
        LOG_ERROR("Client closed connection");
        return (close(fd), false);
    }
    if (n_read == -1)
    {
        LOG_ERROR("Error reading from client fd");
        return (close(fd), false);
    }
    std::string &acc = _clientRequestBuffer[fd];
    acc.append(buf, n_read);
    // if (acc.size() > (size_t)BUF_SIZE)
    // {
    //     cleanupClient(fd, epollfd);
    //     return false;
    // }

    std::string request;
    request.append(buf, n_read);
    RequestState state = analyzeRequest(acc, request);
    if (state == REQ_INCOMPLETE) // needs to check if it is finished
        return true;
    if (state == REQ_BAD){
        _clientResponseBuffer[fd] = 
           "HTTP/1.1 400 BAD REQUEST\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 241\r\n\r\n"
            "<html>\r\n"
            "<head><title>400 - Bad Request</title></head>\r\n"
            "<body>\r\n"
            "<h1>400 - Bad Request</h1>\r\n"
            "<p>-____-The request seems to be incorrect.-____-</p>\r\n"
            "</body>\r\n"
            "</html>\r\n\r\n";
        ev.events = EPOLLOUT;
        ev.data.fd = fd;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, NULL);
        // cleanupClient(fd, epollfd);
        return true;
    }
    else if (state == REQ_BAD_411){
        _clientResponseBuffer[fd] = 
            "HTTP/1.1 411 LENGTH REQUIRED\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 265\r\n\r\n"
            "<html>\r\n"
            "<head><title>411 LENGTH REQUIRED</title></head>\r\n"
            "<body>\r\n"
            "<h1>411 - Length Required</h1>\r\n"
            "<p>-___-We need the Content-Length to answer this request.-___-</p>\r\n"
            "</body>\r\n"
            "</html>\r\n\r\n";
        ev.events = EPOLLOUT;
        ev.data.fd = fd;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
        return true;
    }
    else if (state == REQ_BAD_413){
        _clientResponseBuffer[fd] = 
            "HTTP/1.1 413 CONTENT TOO LARGE\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 253\r\n\r\n"
            "<html>\r\n"
            "<head><title>413 CONTENT TOO LARGE</title></head>\r\n"
            "<body>\r\n"
            "<h1>413 - Content Too Large</h1>\r\n"
            "<p>-___-Request is too large to process.-___-</p>\r\n"
            "</body>\r\n"
            "</html>\r\n\r\n";
        ev.events = EPOLLOUT;
        ev.data.fd = fd;
        epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
        return true;
    }
    _clientRequestBuffer.erase(fd);

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

    std::string requestLine = request.substr(0, request.find("\r\n"));

    const std::string &serverName = serverConfig->getServerName();
    LOG_INFO("[" + (serverName.empty() ? listener->getService() : serverName) + "] " + requestLine);

    RequestHandler requestHandler(request, *serverConfig);
    if (requestHandler.handleRequest(*listener) == false)
    {
        LOG_ERROR("handleRequest failed, sending 500");
        _clientResponseBuffer[fd] =
        "HTTP/1.1 500 Internal Server Error\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 237\r\n\r\n"
        "<html>\r\n"
        "<head><title>500 Internal Server Error</title></head>\r\n"
        "<body>\r\n"
        "<h1>Internal Server Error</h1>\r\n"
        "<p>-___-The server failed -___-</p>\r\n"
        "</body>\r\n"
        "</html>\r\n\r\n";
        
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
            _clientResponseBuffer[fd] = requestHandler.buildAlternativErrorPage("502");
        
            ev.events = EPOLLOUT;
            ev.data.fd = fd;
            epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
            
            return true;
        }
        ev.events = EPOLLOUT;
        ev.data.fd = cgi->getStdinFd();
        epoll_ctl(epollfd, EPOLL_CTL_ADD, cgi->getStdinFd(), &ev);
        _fdToCGI[cgi->getStdinFd()] = cgi;

        ev.events = EPOLLIN;
        ev.data.fd = cgi->getStdoutFd();
        epoll_ctl(epollfd, EPOLL_CTL_ADD, cgi->getStdoutFd(), &ev);
        _fdToCGI[cgi->getStdoutFd()] = cgi;
        
        return true;
    }

    std::ostringstream dbg;
    // ------------ Debug ------------
    dbg << "\nResponse header:" << fd;
    LOG_DEBUG(dbg.str());

    _clientResponseBuffer[fd] += requestHandler.getHeader();
    _clientResponseBuffer[fd] += std::string(
        requestHandler.getBuffer().c_str(),
        requestHandler.getNReadIndex());

    ev.events = EPOLLOUT;

    ev.data.fd = fd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, fd, &ev);
    
    return true;

}

void EpollLoop::cleanupCGI(CGI *cgi, int epollfd, std::string const& response,
    epoll_event &ev)
{
    int inFd  = cgi->getStdinFd();
    int outFd = cgi->getStdoutFd();

    if (inFd != -1)
    {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, inFd, NULL);
        _fdToCGI.erase(inFd);
    }
    if (outFd != -1)
    {
        epoll_ctl(epollfd, EPOLL_CTL_DEL, outFd, NULL);
        _fdToCGI.erase(outFd);
    }

    int clientFd = cgi->getClientFd();
    _clientResponseBuffer[clientFd] += response;

    ev.events  = EPOLLOUT;
    ev.data.fd = clientFd;
    epoll_ctl(epollfd, EPOLL_CTL_MOD, clientFd, &ev);

    delete cgi;
}

void EpollLoop::checkCGITimeout(int epollfd, epoll_event &ev)
{
    if (_fdToCGI.empty())
        return;

    time_t now = time(NULL);
    std::vector<CGI*> expired;
    for (std::map<int, CGI*>::iterator it = _fdToCGI.begin(); it != _fdToCGI.end(); ++it)
    {
        CGI *cgi = it->second;
        if (cgi->hasTimedOut(now) == false)
            continue;
        if (std::find(expired.begin(), expired.end(), cgi) == expired.end())
            expired.push_back(cgi);
    }

    for (size_t i = 0; i < expired.size(); ++i)
    {
        LOG_ERROR("CGI: script exceeded timeout, killing it");
        cleanupCGI(expired[i], epollfd,
            "HTTP/1.1 504 Gateway Timeout\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 235\r\n\r\n"
            "<html>\r\n"
            "<head><title>504 Gateway Timeout </title></head>\r\n"
            "<body>\r\n"
            "<h1>504 Gateway Timeout</h1>\r\n"
            "<p>-___-Script took too long -___-</p>\r\n"
            "</body>\r\n"
            "</html>\r\n\r\n", ev);
    }
}

bool EpollLoop::do_write_fd( int fd, int epollfd, epoll_event &ev ) {
     
    std::string & response = _clientResponseBuffer[fd];
    if (response.empty())
        return false;
    
    // LOG_INFO(COLOR_GREEN + std::string("response : ") + response  + COLOR_RESET);

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
    
    if (headerSent == 0)
    {
        LOG_INFO("Message completely sent.");
        
        epoll_ctl(epollfd, EPOLL_CTL_DEL, fd, NULL);
        close(fd);
        _clientResponseBuffer.erase(fd);
        _clientToListener.erase(fd);
        
        return true;
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

            if (listenerSockfd != -1) 
            {
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
            
            } 
            else 
            {    
                std::map<int, CGI*>::iterator it =
                    _fdToCGI.find(events[n].data.fd);
                if (it != _fdToCGI.end())
                {
                    CGI *cgi = it->second;
                    int activeFd = events[n].data.fd;
                    if (activeFd == cgi->getStdinFd())
                    {
                        Result w = cgi->onWritable();
                        if (w == SUCCESS)
                        {
                            epoll_ctl(epollfd, EPOLL_CTL_DEL, activeFd, NULL);
                            _fdToCGI.erase(activeFd);
                            cgi->closeStdin();
                        }
                        else if (w == ERR)
                        {
                            LOG_ERROR("CGI: write failed; couldn't send request body\n");
                            cleanupCGI(cgi, epollfd,
                                "HTTP/1.1 502 Bad Gateway\r\n"
                                "Content-Type: text/html\r\n"
                                "Content-Length: 161\r\n\r\n"
                                "<html>\r\n"
                                "<head><title>502 Bad Gateway EPOLL WRITE</title></head>\r\n"
                                "<body>\r\n"
                                "<h1>502 Bad Gateway</h1>\r\n"
                                "<p>-___-Took wrong turn somewhere -___-</p>\r\n"
                                "</body>\r\n"
                                "</html>\r\n\r\n", ev);
                        }
                        //hold
                    }
                    else
                    {
                        Result r = cgi->onReadable();
                        if (r == SUCCESS)
                            cleanupCGI(cgi, epollfd, cgi->buildResponse(), ev);
                        else if (r == ERR)
                        {
                            LOG_ERROR("CGI: read from script failed; couldn't respond to client\n");
                            cleanupCGI(cgi, epollfd,
                                "HTTP/1.1 502 Bad Gateway\r\n"
                                "Content-Type: text/html\r\n"
                                "Content-Length: 161\r\n\r\n"
                                "<html>\r\n"
                                "<head><title>502 Bad Gateway EPOLL READ</title></head>\r\n"
                                "<body>\r\n"
                                "<h1>502 Bad Gateway</h1>\r\n"
                                "<p>-___-Took wrong turn somewhere -___-</p>\r\n"
                                "</body>\r\n"
                                "</html>\r\n\r\n", ev);
                        }
                        //hold
                    }

                }
                else 
                {
                    
                    if ( events[n].events & EPOLLIN ) 
                    {  

                        if (!do_read_fd(events[n].data.fd,
                                listeners, config, epollfd, ev))
                            break;
                    
                    } 
                    else if ( events[n].events & EPOLLOUT ) 
                    {

                        if (!do_write_fd(events[n].data.fd,
                                epollfd, ev))
                            break;
                    }
                }
            }
        }
        checkCGITimeout(epollfd, ev);
    }

    close(epollfd);

    return true;
}
