/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:00:06 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/02 19:00:13 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
#include "HTTPParser.hpp"
#include "CGIHandler.hpp"

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
    std::string         _root;
    std::string         _header;
    std::string         _size;
    char                _buffer[BUF_SIZE];
    ssize_t             _n_read_index;
    /*CGI */
    bool                _isCGI;
    std::string         _fullPath; // location.root + _scriptFilename
    std::string         _query_string;
    std::string         _scriptFilename;
    std::string         _body;
    std::string         _content_type;
    std::string         _content_length;
    std::string         _method;
   

    
protected:
public:

                        RequestHandler( std::string const & request );
                        RequestHandler( RequestHandler const & src );
                        ~RequestHandler();
    RequestHandler &    operator=( RequestHandler const & other );

    std::string         getBuffer() const;
    std::string         getHeader() const;
    std::string         getSize() const;
    ssize_t             getNReadIndex() const;

    
    std::string         buildAnswerHeader( std::string const & code, std::string const & type );
    
    std::string         getFile( std::string const & code );
    std::string         getFileImage( std::string const & code);
    std::string         getFileUpload( std::string const & code);

    bool                answerFile( std::string const & file );
    bool                answerFileImage( std::string const & file );
    bool                answerFileIcon();

    bool                uploadFile();

    bool                handleRequest(  ListenerManager const & listen );
    bool                getCGI() const;
    std::string     RequestHandler::getPath() const{
        // need root to construct
    }

    std::string     RequestHandler::getFilename() const {
        return _scriptFilename;
    }

    std::string     RequestHandler::getQueryString() const {
        return _query_string;
    }
    std::string     RequestHandler::getBody() const{
        return _body;
    }
    std::string     RequestHandler::getContentType() const{
        return _content_type;
    }
    std::string     RequestHandler::getContentLength() const{
        return _content_length;
    }
    std::string     RequestHandler::getMethod() const {
        return _method;
    }

};