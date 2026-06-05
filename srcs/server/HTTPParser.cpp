/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:20:07 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/05 15:23:17 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPParser.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

HTTPParser:: HTTPParser( std::string request) : _request(request), _buffer(""), _n_read_index(0){
    
}

HTTPParser::HTTPParser( HTTPParser const & src ) : _request(src._request), _n_read_index(src._n_read_index){
    
     _buffer[BUF_SIZE] = src._buffer[BUF_SIZE];
}

HTTPParser::~HTTPParser(){
    
}

HTTPParser &    HTTPParser::operator=( HTTPParser const & other ){

    if (this != &other ){

        this->_request = other._request;
        this->_buffer[BUF_SIZE] = other._buffer[BUF_SIZE];
        this->_n_read_index = other._n_read_index;

    }
    
    return *this;
}

std::string HTTPParser::getBuffer() const{

    return _buffer;
}

int HTTPParser::getNReadIndex() const{
    
    return _n_read_index;
}

/*
** ============================================================================
** Parser HTTP
** ============================================================================
*/

bool    HTTPParser::findMethods(){
    
    if (_request.find("GET / HTTP/1.1") != std::string::npos){ // same as EOF
        
       
        int indexfd = open("data/html/index.html", O_RDONLY);
        this->_n_read_index = read(indexfd, _buffer, BUF_SIZE);
        close(indexfd);
        std::cout << "n_read_index:" << _n_read_index << std::endl;
        if (_n_read_index == -1)
            return false;
    }
    
    return true;
        
}

bool    HTTPParser::findPath(){

    return true;
}

bool    HTTPParser::findHeaders(){
    
    return true;
    
}

bool    HTTPParser::findCGI(){
    
    return true;
    
}
