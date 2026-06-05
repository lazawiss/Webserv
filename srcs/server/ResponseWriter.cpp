/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseWriter.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:46:22 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/05 15:41:04 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ResponseWriter.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

ResponseWriter:: ResponseWriter( std::string response, int fd ) : _response(response), _fd(fd){
    
}

ResponseWriter::ResponseWriter( ResponseWriter const & src ) : _response(src._response), _fd(src._fd){
    
}

ResponseWriter::~ResponseWriter(){
    
    // if (_fd)
    //     close(_fd);
}

ResponseWriter &    ResponseWriter::operator=( ResponseWriter const & other ){
    
    if (this != &other){
        
        this->_response = other._response;
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
    
    if (send(_fd, _response.c_str(), _response.size(), 0) < 0)
        return false;
            
    return true;
}
