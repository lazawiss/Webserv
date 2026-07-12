/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:00:06 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/12 16:55:21 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
#include "HTTPParser.hpp"
#include "../parser/config/ServerConfig.hpp"


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
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <limits>


#define BUF_SIZE 800000


class RequestHandler {
  
private:

    std::string         _request;
    const ServerConfig  &_serverConfig;
    std::string         _root;
    std::string         _header;
    std::string         _size;
    std::string         _pathToFile;
    char                _buffer[BUF_SIZE];
    ssize_t             _n_read_index;
   

    
protected:
public:

                        RequestHandler( std::string const & request, const ServerConfig &serverConfig );
                        RequestHandler( RequestHandler const & src );
                        ~RequestHandler();
    RequestHandler &    operator=( RequestHandler const & other );

    std::string         getBuffer() const;
    std::string         getHeader() const;
    std::string         getSize() const;
    ssize_t             getNReadIndex() const;

    
    std::string         buildAnswerHeader( std::string const & code, std::string const & type );
    
    std::string         getFile( std::string const & code, bool const & error );
    std::string         getFileImage( std::string const & code);
    std::string         getFileUpload( std::string const & code);

    bool                answerFile( std::string const & file );
    bool                answerFileImage( std::string const & file );
    bool                answerFileIcon();

    bool                uploadFile( std::string const & filename, std::string const & buf );
    bool                removeFile( std::string const & filename );


    bool                handleRequest(  ListenerManager const & listen );

};