/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 15:54:09 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/07 18:24:48 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# pragma once

#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"


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


#define BUF_SIZE 800000


class HTTPParser {

private:

    std::vector<Token>              _allTokens;
    std::string                     _request;
    std::string                     _code;
    std::string                     _type;
    std::string                     _method;
    std::string                     _requesttarget;
    std::string                     _httpversion;
    std::string                     _boundary;
    std::string                     _fileLength;
    std::string                     _fileName;
    std::string                     _fileBuf;
    std::vector<Token>::iterator    _found;
    std::string::iterator           _pos;
    
    
    

public:

                                HTTPParser( std::string const & request );
                                HTTPParser( HTTPParser const & src );    
                                ~HTTPParser();    
    HTTPParser &                operator=( HTTPParser const & other );
            
    std::string                 getCode() const;
    std::string                 getType() const;
    std::string                 getMethod() const;
    std::string                 getBoundary() const;
    std::string                 getFileName() const;
    std::string                 getFileBuf() const;
            
    std::string                 setCode( std::string const & code );
    std::string                 setType( std::string const & type );

    std::vector<size_t> &       collectSpace( std::string::iterator pos );
    std::vector<std::string> &  collectString( std::vector<size_t> space_inter );


    bool                        checkSize();
    bool                        checkRequestLine();
    bool                        checkHost( ListenerManager const & listener );
    bool                        isRequestValid( ListenerManager const & listen );
                
    bool                        checkContentType();
    bool                        findBoundary();
    bool                        checkContentLength();
    bool                        checkContentDisposition();
    bool                        gatherFile();
            
            
    // v    oid                    HTTPparse_file(const std::string& path);
                
                
    bool                        findMethods();
    bool                        findPath();
    bool                        findHeaders();
    bool                        findCGI();

};