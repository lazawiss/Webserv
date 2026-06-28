/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenerManager.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:27:13 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/04 17:10:49 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

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
#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <limits>


class ListenerManager {

private:

protected:

    struct addrinfo _hints, *_res, *_p;
    int             _sockfd;
    std::string     _node;
    std::string     _service;
    
    

public:

                        ListenerManager();
                        ListenerManager( const std::string &host, const std::string &port );
                        ~ListenerManager();

private:
                        ListenerManager( ListenerManager const & );
    ListenerManager &   operator=( ListenerManager const & );

    int                 getSockfd() const;
    std::string         getNode() const;
    std::string         getService() const;

    
    struct addrinfo &   initHints();
    bool                initRes();
            
    bool                loopBindingSocket();
    bool                listeningSocket();
    void                releaseSockfd();

};

