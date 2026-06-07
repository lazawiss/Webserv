/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   EpollLoop.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 13:27:21 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/07 13:29:49 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

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
#include "ListenerManager.hpp"
#include "HTTPParser.hpp"
#include "ResponseWriter.hpp"
#include "../parser/Parser.hpp"
#include "../lexer/Lexer.hpp"

class Server;


class EpollLoop {

private:

protected:

public:

                EpollLoop();
                EpollLoop( EpollLoop const & src );
                ~EpollLoop();
    EpollLoop & operator=( EpollLoop const & other );

    int         setnonblocking( int fd );
    bool        do_use_fd( int fd );
    bool        readingSocket( ListenerManager const & listen );


};