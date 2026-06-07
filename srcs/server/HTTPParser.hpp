/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 15:54:09 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/05 20:22:34 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include <iostream>
#include <istream>
#include <ios>

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


class HTTPParser {

private:

    std::string _request;
    std::string _root;
    std::string _index;
    std::string _error;
    char        _buffer[BUF_SIZE];
    ssize_t     _n_read_index;

protected:

public:

                    HTTPParser( std::string request);
                    HTTPParser( HTTPParser const & src );    
                    ~HTTPParser();    
    HTTPParser &    operator=( HTTPParser const & other );

    std::string     getBuffer() const;
    int             getNReadIndex() const;


    bool            findMethods();
    bool            findPath();
    bool            findHeaders();
    bool            findCGI();

    std::string     getFile200();
    std::string     getFile404();
    std::string     getFile414();
    
    bool            answerFile( std::string file );
};