/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: leazannis <leazannis@student.42.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:27:21 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/25 19:15:19 by leazannis        ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include "ListenerManager.hpp"
#include "HTTPParser.hpp"
#include "../parser/Parser.hpp"
#include "../lexer/Lexer.hpp"
#include "RequestHandler.hpp"
#include "../parser/config/GlobalConfig.hpp"
#include "CGIHandler.hpp"

#include <fstream>
#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <limits>


class Server;

class CGI;

class EpollLoop {

private:
    std::map<int, int>          _clientToListener; // key value lookup
    // - key : client socketfd, - value: listener socketfd
    std::map<int, CGI*>         _fdToCGI;  // pipe fd, CGI value
    std::map<int, std::string>  _clientResponseBuffer;  // int clientfd, std::string response, like Request
    std::map<int, std::string>  _clientRequestBuffer;
    
    std::string                 _header;
    std::string                 _content;

    
protected:

public:

                EpollLoop();
                EpollLoop( EpollLoop const & src );
                ~EpollLoop();
    EpollLoop & operator=( EpollLoop const & other );

    int         setnonblocking( int fd );

    bool        do_read_fd( int fd, std::vector<ListenerManager*> const & listeners, const GlobalConfig &config, int epollfd , epoll_event& ev);
    bool        do_write_fd( int fd, int epollfd, epoll_event &ev );

    void        cleanupClient( int fd, int epollfd );
    
    // bool        do_use_fd( int fd, std::vector<ListenerManager*> const & listeners, const GlobalConfig &config, int epollfd , epoll_event& ev);
    bool        readingSocket( std::vector<ListenerManager*> const & listeners, const GlobalConfig &config );
 
};