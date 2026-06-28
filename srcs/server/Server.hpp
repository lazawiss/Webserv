/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 14:12:24 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/23 13:38:47 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "SignalManager.hpp"
#include "ListenerManager.hpp"
#include "EpollLoop.hpp"
#include "../parser/config/GlobalConfig.hpp"

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

#include <sys/time.h>
#include <sstream>
#include <iomanip>

std::string logTimestamp();

#define LOG_SYSTEM(msg) std::cout << logTimestamp() << " [System]  " << msg << std::endl
#define LOG_INFO(msg)   std::cout << logTimestamp() << " [Info]    " << msg << std::endl
#define LOG_DEBUG(msg)  std::cout << logTimestamp() << " [Debug]   " << msg << std::endl
#define LOG_ERROR(msg)  std::cerr << logTimestamp() << " [Error]   " << msg << std::endl
#define LOG_SEP()       std::cout << logTimestamp() << " ---------------------------------------------------" << std::endl

#define BUF_SIZE 800000
#define LISTEN_BACKLOG 50 //max connections accepted by socket
#define MAX_EVENTS 10

class SignalManager;
class EpollLoop;


class Server
{

private:
    
    GlobalConfig                    _config;
    SignalManager                   _signalManager;
    std::vector<ListenerManager*>   _listenermanagers;
    EpollLoop                       _epollloop;

protected:
public:
    
    static volatile sig_atomic_t    _quit;

                //Server();
                Server( const GlobalConfig &config );
                Server( Server const & src );
                ~Server();
    Server &    operator=( Server const & other );

    void        start();
    void        run();

};
