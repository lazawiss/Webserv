/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseWriter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:46:22 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/05 17:35:42 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseWriter.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

ResponseWriter:: ResponseWriter( std::string header, std::string content, int fd ) : _header(header),_content(content), _fd(fd){
    
}

ResponseWriter::ResponseWriter( ResponseWriter const & src ) : _header(src._header), _content(src._content), _fd(src._fd){
    
}

ResponseWriter::~ResponseWriter(){
    
}

ResponseWriter &    ResponseWriter::operator=( ResponseWriter const & other ){
    
    if (this != &other){
        
        this->_header = other._header;
        this->_content = other._content;
        this->_fd = other._fd;
    }

    return *this;
}

/*
** ============================================================================
** Response Writer
** ============================================================================
*/

bool ResponseWriter::sendResponse(){
    
    if (send(_fd, _header.c_str(), _header.size(), 0) < 0)
        return false;
    if (send(_fd, _content.c_str(), _content.size(), 0) < 0)
        return false;
            
    return true;
}
