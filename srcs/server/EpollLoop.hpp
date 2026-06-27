/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:27:21 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/27 16:13:16 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include "ListenerManager.hpp"
#include "HTTPParser.hpp"
#include "ResponseSender.hpp"
#include "../parser/Parser.hpp"
#include "../lexer/Lexer.hpp"
#include "RequestHandler.hpp"
#include "CGI.hpp"

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

class EpollLoop {

private:
    std::map<int, int>  _clientToListener;

protected:

public:

                EpollLoop();
                EpollLoop( EpollLoop const & src );
                ~EpollLoop();
    EpollLoop & operator=( EpollLoop const & other );

    int         setnonblocking( int fd );
    bool        do_use_fd( int fd, ListenerManager const & listen );
    bool        readingSocket( ListenerManager const & listen );

    bool        do_use_fd( int fd, std::vector<ListenerManager> const & listeners );
    bool        readingSocket( std::vector<ListenerManager> const & listeners );

};