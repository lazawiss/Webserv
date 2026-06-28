/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:03:45 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/28 15:24:15 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
#include "HTTPParser.hpp"
#include "RequestHandler.hpp"

/*
** ============================================================================
** Constructors & Destructor & Getters
** ============================================================================
*/

RequestHandler::RequestHandler( std::string const & request ) : _request(request),
_root("data/html"), _header(), _size(), _n_read_index(0){

    memset(_buffer, 0, BUF_SIZE);
}

RequestHandler::RequestHandler( RequestHandler const & src ) : 
    _request(src._request), _root(src._root), _header(src._header),
    _size(src._size),_n_read_index(src._n_read_index)
{
     memcpy(_buffer, src._buffer, BUF_SIZE);
}

RequestHandler::~RequestHandler(){}

RequestHandler & RequestHandler::operator=( RequestHandler const & other ){
    
    if ( this != &other){
        this->_request = other._request;
        this->_root = other._root;
        this->_header = other._header;
        this->_size = other._size;
         memcpy(this->_buffer, other._buffer, BUF_SIZE);
        this->_n_read_index = other._n_read_index;
    }

    return *this;
}

std::string RequestHandler::getBuffer() const{

    return std::string(_buffer, _n_read_index);
}

std::string RequestHandler::getHeader() const{

    return _header;
}

std::string RequestHandler::getSize() const{

    return _size;
}


ssize_t RequestHandler::getNReadIndex() const{
    
    return _n_read_index;
}

/*
** ============================================================================
** Request Handler
** ============================================================================
*/

// build path toward file
std::string RequestHandler::getFile( std::string const & code ){
    
    std::string file = _root;
    file += "/";
    file += code;
    file += ".";
    file += "html";
 
    std::cout << "File:" << file << std::endl;
    return file;
}

// build path toward file
std::string RequestHandler::getFileImage( std::string const & code){
    
    std::string file = "data/images";
    file += "/";
    file += code;
    // file += ".";
    // if (type == "image/jpeg")
    //     file += "jpeg";
    // if (type == "image/png")
    //     file += "png";
    // if (type == "image/gif")
    //     file += "gif";
 
    std::cout << "FileImage:" << file << std::endl;
    return file;
}
 
// construct message to send back to client 
//  header : code + Content-Type
std::string RequestHandler::buildAnswerHeader( std::string const & code, std::string const & type ){
    
    //cherche dans tableau >> code + reason
    std::string codeName[8] = {
        "index",
        "image",
        "400",
        "404",
        "405",
        "413",
        "414",
        "421"
    };
    
    int index = 0;
    for (int i = 0 ;i < 8; i++){
        
        if (codeName[i] == code){
            index = i;
            break;
        }
    }
    
    std::string str;
    switch (index){
        
        case(0):
        str = "200 OK";
        break;

        case(1):
        str = "200 OK";
        break;

        case(2):
        str = "400 BAD REQUEST";
        // str = "302 FOUND\r\nLocation: /html/400.html";

        break;

        case(3):
        str = "404 Not Found";
        break;

        case(4):
        str = "405 METHOD NOT ALLOWED\r\nAllow: GET, POST, DELETE";
        break;
        
        case(5):
        str = "413 CONTENT TOO LARGE";
        break;
        
        case(6):
        str = "414 URI TOO LONG";
        break;
        
        case(7):
        str = "421 MISDIRECTED REQUEST";
        break;

        default:
        break;
        
    }
    // stocke dans string 
    
    _header = "HTTP/1.1 ";
    _header += str;
    _header += "\r\n";
    _header += "Content-Type: ";
    _header += type;
    _header += "\r\n";
    _header += "Content-Length: ";
    // if ( type == "text/html"){
        
        std::stringstream ss;
        ss << _n_read_index;
        std::string size = ss.str();
        _header += size;
    // }
    // if ( type == "image/jpeg" ||  type == "image/png" || type == "image/gif" || type == "image/x-icon"){
        
    //     _header += _size;
    //     _header += "\r\n";
    //     _header += "Connection: keep-alive";
    //     // _header += "Connection: close";

    // }
    // if (atoi(str.c_str()) >= 400){
        
        // _header += "Cache-Control: no-store, no-cache, must-revalidate, max-age=0\r\n";
        // _header += "Pragma: no-cache\r\n";
        // _header += "Expires: 0\r\n";
    // }
    _header += "\r\n\r\n";
    
    return _header;
}

// open file + stock it in buffer to send back to client
// content = text
bool    RequestHandler::answerFile( std::string const & file ){

    struct stat sb;
    
    if (stat(file.c_str(), &sb) == -1){
        std::cerr << "Error stat: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "File Size : " << sb.st_size <<std::endl;
    
    int indexfd = open(file.c_str(), O_RDONLY);
    if (indexfd == -1){
        std::cerr << "Error file failed to open on indexfd:" << indexfd << std::endl;
        close(indexfd);
        return false;
    }
    this->_n_read_index = read(indexfd, _buffer, BUF_SIZE);
    close(indexfd);
    std::cout << "n_read_index:" << _n_read_index << std::endl;
    if (_n_read_index == -1)
        return false;
    
    return true;
    
}

// open file + stock it in buffer to send back to client
// content = image
bool    RequestHandler::answerFileImage( std::string const & file ){

    struct stat sb;
    
    if (stat(file.c_str(), &sb) == -1){
        std::cerr << "Error stat: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "File Size : " << sb.st_size <<std::endl;

    int indexfd = open(file.c_str(), O_RDONLY);
    if (indexfd == -1){
        std::cerr << "Error file failed to open on indexfd:" << indexfd << std::endl;
        close(indexfd);
        return false;
    }
    
    _n_read_index = read(indexfd, _buffer, BUF_SIZE);
    close(indexfd);
    std::cout << "n_read_index:" << _n_read_index << std::endl;
    if (_n_read_index == -1)
        return false;

    if (_n_read_index > BUF_SIZE){
        std::cerr << "Image size is too big." << std::endl;
        return false;
    }

    return true;
}

// open file + stock it in buffer to send back to client
// content = x-icon
bool    RequestHandler::answerFileIcon(){

    struct stat sb;
    
    if (stat("data/favicon.ico/favicon-16x16.png", &sb) == -1){
        std::cerr << "Error stat: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "File Size : " << sb.st_size <<std::endl;

    int indexfd = open("data/favicon.ico/favicon-16x16.png", O_RDONLY);
    if (indexfd == -1){
        std::cerr << "Error file failed to open on indexfd:" << indexfd << std::endl;
        close(indexfd);
        return false;
    }
    _n_read_index = read(indexfd, _buffer, BUF_SIZE);
    close(indexfd);
    std::cout << "n_read_index:" << _n_read_index << std::endl;
    if (_n_read_index == -1)
        return false;

    if (_n_read_index > BUF_SIZE){
        std::cerr << "Image size is too big." << std::endl;
        return false;
    }
    
    return true;
}

// main function : 
// instanciate HTTPParser 
// checks if request valid
// parse request
// find proper file to send to client
// build answer depending of content to sent
bool    RequestHandler::handleRequest(  ListenerManager const & listen ){
    
    HTTPParser HTTPParser(_request);

    HTTPParser.HTTPparse_file(_request);
    
    bool    requestValid = true;

    //  check request
    if (HTTPParser.isRequestValid(listen) == false){
        std::cout << "Request Invalid." << std::endl;
        requestValid = false;
    }
    
    /**CGI AJOUT */
    if (requestValid && (HTTPParser.getRequestTarget().find("/cgi-bin/")) != std::string::npos){
        // create CGI object here        
        // parse request 
        // return (CGI object)
        // http parser sets stuff so add a cgi object inside?
        // or just set cgi 
        // 
        HTTPParser.findCGI();
    }

    // find method 
    else if (requestValid == true && HTTPParser.findMethods() == false){
        std::cerr << "Error Method not implemented: " << strerror(errno) << std::endl;
    }

    if (HTTPParser.getType() == "text/html"){
        
        std::string file = getFile(HTTPParser.getCode()); 
        if (answerFile(file) == false){
            
            HTTPParser.setCode("404");
            HTTPParser.setCode("text/html");
        }
            
    }
    if (HTTPParser.getType() == "image/jpeg" || HTTPParser.getType() == "image/png" || HTTPParser.getType() == "image/gif"){
        
        std::string file = getFileImage(HTTPParser.getCode()); 
        if (answerFileImage(file) == false){
         
            HTTPParser.setCode("404");
            HTTPParser.setCode("text/html");
        }
    }
    if (HTTPParser.getType() == "image/x-icon"){
        
        if (answerFileIcon() == false){
         
            HTTPParser.setCode("404");
            HTTPParser.setCode("text/html");
        }
    }
    buildAnswerHeader(HTTPParser.getCode(), HTTPParser.getType());

    return true;

}
