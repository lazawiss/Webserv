/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseWriter.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:40:00 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/04 18:57:59 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

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

#define BUF_SIZE 500

class ResponseWriter {

private:

    std::string _response;
    int         _fd;

protected:

public:

                        ResponseWriter( std::string response, int fd );
                        ResponseWriter( ResponseWriter const & src );
                        ~ResponseWriter();
    ResponseWriter &    operator=( ResponseWriter const & other );

    bool                sendResponse();
    
};