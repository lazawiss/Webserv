/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/21 14:12:24 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/26 15:26:44 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <errno.h>
#include <netdb.h>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstdio>

#define BUF_SIZE 500
#define MY_SOCK_PATH "home/lzannis/Projets/Weberv/data/html/index.html"
#define LISTEN_BACKLOG 50 //max connections accepted by socket

class Server {

private:

    struct addrinfo _hints, *_res, *_p;
    int             _sockfd;
    
protected:
public:
    
                        Server();
                        Server( Server const & src );
                        ~Server();
    
    Server &            operator=( Server const & other );

    struct addrinfo &   initHints();
    bool                initRes();
                
    void                findAddress();
            
    bool                loopBindingSocket();
    bool                listeningSocket();
    void                readingSocket();
    


};
