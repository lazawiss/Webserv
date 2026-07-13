/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 15:54:09 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/13 17:39:13 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# pragma once

#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
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
#include <sys/socket.h>
// #include <sys/epoll.h>
#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <limits>


#define BUF_SIZE 800000


class HTTPParser {

private:

    std::string                     _request;
    const ServerConfig              &_serverConfig;
    std::string                     _code;
    std::string                     _type;
    std::string                     _method;
    std::string                     _requesttarget; // CGI 
    std::string                     _httpversion;
    std::string                     _boundary;
    std::string                     _fileLength;
    std::string                     _fileName;
    std::string                     _fileBuf;
    std::string::iterator           _pos;
    bool                            _errors;
    
/* ADD INS FOR CGI------*/
    bool                _isCGI;
    std::string         _fullPath; // location.root + _scriptFilename
    std::string         _query_string;
    std::string         _scriptFilename;
    std::string         _body;
    std::string         _content_type;
    std::string         _content_length;
    int                 _content_int;

    

public:

                                HTTPParser( std::string const & request, const ServerConfig &serverConfig  );
                                HTTPParser( HTTPParser const & src );    
                                ~HTTPParser();    
    HTTPParser &                operator=( HTTPParser const & other );
            
    std::string                 getCode() const;
    std::string                 getType() const;
    std::string                 getMethod() const;
    std::string                 getBoundary() const;
    std::string                 getFileName() const;
    std::string                 getFileBuf() const;
    bool                        getError() const;

    std::string                 setCode( std::string const & code );
    std::string                 setType( std::string const & type );
    bool                        setError( bool error );    


    /* ADD INS FOR CGI-------------*/
    std::string                 getPath() const;
    std::string                 getFilename() const;
    std::string                 getQueryString() const;
    std::string                 getBody() const;
    std::string                 getContentType() const;
    std::string                 getContentLength() const;
    std::string                 getRequestTarget() const;
    bool                        isCGI() const;
    void                        parseCGI();
    void                        extractBody();
    bool                        validateCGIRequest();
    /*------------------------- */

    std::vector<size_t>         collectSpace( std::string::iterator pos );
    std::vector<std::string>    collectString( std::vector<size_t> space_inter );

    bool                        checkSize();
    bool                        checkRequestLine();
    bool                        checkHost( ListenerManager const & listener );
    bool                        isRequestValid( ListenerManager const & listen );
                
    bool                        checkContentType();
    bool                        checkContentLength();
    bool                        checkContentDisposition();
    bool                        gatherFile();
            
    std::string                 addSuffix(std::string suffix);
                
    bool                        findMethods();
    bool                        findPath();
    bool                        findHeaders();
};