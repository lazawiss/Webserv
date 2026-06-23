/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 15:54:09 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/23 13:52:49 by lzannis          ###   ########.fr       */
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

    std::vector<Token>  _allTokens;
    std::string         _request;
    std::string         _code;
    std::string         _type;
    std::string         _method;
    std::string         _requesttarget;
    std::string         _httpversion;

    
protected:

public:

                    HTTPParser( std::string const & request );
                    HTTPParser( HTTPParser const & src );    
                    ~HTTPParser();    
    HTTPParser &    operator=( HTTPParser const & other );

    std::string     getCode() const;
    std::string     getType() const;
    std::string     getMethod() const;
    
    std::string     setCode( std::string const & code );
    std::string     setType( std::string const & type );

    

    bool            checkSize();
    bool            checkRequestLine();
    bool            checkHost( ListenerManager const & listener );
    bool            isRequestValid( ListenerManager const & listen );


    void            HTTPparse_file(const std::string& path);
    
    
    bool            findMethods();
    bool            findPath();
    bool            findHeaders();
    bool            findCGI();

};