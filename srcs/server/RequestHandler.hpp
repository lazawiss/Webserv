/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:00:06 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/09 21:37:27 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
#include "HTTPParser.hpp"


#include <iostream>
#include <fstream>
#include <sstream>
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


class RequestHandler {
  
private:

    std::vector<Token>  _listTokens;
    std::string         _root;
    std::string         _header;
    std::string         _size;
    char                _buffer[BUF_SIZE];
    ssize_t             _n_read_index;

    
protected:
public:

                        RequestHandler( std::vector<Token> listTokens );
                        RequestHandler( RequestHandler const & src );
                        ~RequestHandler();
    RequestHandler &    operator=( RequestHandler const & other );

    std::string         getBuffer() const;
    std::string         getHeader() const;
    std::string         getSize() const;
    int                 getNReadIndex() const;

    
    std::string         buildAnswerHeader( std::string code, std::string type );
    std::string         getFile( std::string code );
    bool                answerFile( std::string file );
    bool                answerFileImage();

    
    bool                handleRequest(  ListenerManager const & listen );

};