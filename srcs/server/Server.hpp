/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 14:12:24 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/07 14:04:14 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "SignalManager.hpp"
#include "ListenerManager.hpp"
#include "EpollLoop.hpp"

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



#define BUF_SIZE 8000
#define MY_SOCK_PATH "home/lzannis/Projets/Weberv/data/html/index.html"
#define LISTEN_BACKLOG 50 //max connections accepted by socket
#define MAX_EVENTS 10

class SignalManager;
class EpollLoop;


class Server {

private:

    // Lexer   lexer;
    // Parser parser;
    
    SignalManager   _signalManager;
    ListenerManager _listenermanager;
    EpollLoop       _epollloop;

    
protected:
public:
    
    static volatile sig_atomic_t    _quit;

                Server();
                Server( Server const & src );
                ~Server();
    Server &    operator=( Server const & other );

    void        start();
    void        run();

};
