/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.hpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:00:06 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/26 16:02:16 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
#include "HTTPParser.hpp"
#include "../parser/config/ServerConfig.hpp"
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

#define BUF_SIZE 1048576

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
    bool                _alterError;
    /*CGI */
    bool                _isCGI;
    std::string         _fullPath;
    std::string         _query_string;
    std::string         _scriptFilename;
    std::string         _body;
    std::string         _content_type;
    std::string         _content_length;
    std::string         _method;
    std::string         _pathInfo;
    std::string         _scriptName; 
    std::string         _interpreter; 

public:

                        RequestHandler( std::string const & request, const ServerConfig &serverConfig );
                        RequestHandler( RequestHandler const & src );
                        ~RequestHandler();
    RequestHandler &    operator=( RequestHandler const & other );

    std::string         getBuffer() const;
    std::string         getHeader() const;
    std::string         getSize() const;
    ssize_t             getNReadIndex() const;

    std::string         generateAutoindex(const std::string &fullPath, const std::string &requestTarget);
    std::string         buildAnswerHeader( std::string const & code, std::string const & type );
    std::string         buildAlternativErrorPage( std::string const & code );
    
    
    std::string         getFile( std::string const & code, bool const & error );
    std::string         getFileImage( std::string const & code);
    std::string         getFileUpload( std::string const & code);
    
    std::string         getPath() const;
    std::string         getPathInfo() const;
    std::string         getScriptName() const;
    std::string         getInterpreter() const;
    std::string         getFilename() const;
    std::string         getQueryString() const;
    std::string         getBody() const;
    std::string         getContentType() const;
    std::string         getContentLength() const;
    std::string         getMethod() const;

    // Partial resposnes: byte range responses
    // valid - range is parsed and satisifable, 206
    // invalid - range was given but out of bounds, 416
    // if neither, then no usable range set, so 200 as per usuug

    struct ByteRange {
        long start;
        long end;
        bool valid;
        bool unsatisfiable;
        ByteRange() : start(0), end(0), valid(false), unsatisfiable(false) {}
    };

    ByteRange           parseRangeHeader( std::string const & rangeValue, long fileSize );
    bool                answerFilePartial( std::string const & file, ByteRange const & r );
    std::string         buildPartialHeader( std::string const & type, ByteRange const & r, long fileSize );
    std::string         build416Header( long fileSize );
///
    bool                answerFile( std::string const & file );
    bool                answerFileIcon();

    bool                uploadFile( std::string const & filename, std::string const & buf );
    bool                removeFile( std::string const & filename );

    
    void                sendError( HTTPParser & parser, HttpCode code = HTTP_404 );

    bool                handleRequest(  ListenerManager const & listen );
    bool                getCGI() const;

    void                ErrorPage400();
    void                ErrorPage403();
    void                ErrorPage404();
    void                ErrorPage405();
    void                ErrorPage411();
    void                ErrorPage413();
    void                ErrorPage414();
    void                ErrorPage421();
    void                ErrorPage500();
    void                ErrorPage502();
    void                ErrorPage504();
    
};