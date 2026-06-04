/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ListenerManager.hpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:27:13 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/04 15:00:08 by lzannis          ###   ########.fr       */
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

public:

                        ListenerManager();
                        ListenerManager( ListenerManager const & src );
                        ~ListenerManager();
    ListenerManager &   operator=( ListenerManager const & other );

    int                 getSockfd() const;
    
    struct addrinfo &   initHints();
    bool                initRes();
                
    void                findAddress();
            
    bool                loopBindingSocket();
    bool                listeningSocket();


};

