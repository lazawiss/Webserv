/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:03:45 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/16 14:24:40 by lzannis          ###   ########.fr       */
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

RequestHandler:: RequestHandler( std::vector<Token> listTokens ) : _listTokens(listTokens),
_root("data/html"), _header(), _size(), _buffer(""), _n_read_index(0){
    
}

RequestHandler::RequestHandler( RequestHandler const & src ) : _listTokens(src._listTokens),
_root(src._root),_header(src._header), _size(src._size),_n_read_index(src._n_read_index){
    
    _buffer[BUF_SIZE] = src._buffer[BUF_SIZE];

}

RequestHandler::~RequestHandler(){
    
}

RequestHandler & RequestHandler::operator=( RequestHandler const & other ){
    
    if ( this != &other){
        this->_listTokens = other._listTokens;
        this->_root = other._root;
        this->_header = other._header;
        this->_size = other._size;
        this->_buffer[BUF_SIZE] = other._buffer[BUF_SIZE];
        this->_n_read_index = other._n_read_index;
    }

    return *this;
}

std::string RequestHandler::getBuffer() const{

    return _buffer;
}

std::string RequestHandler::getHeader() const{

    return _header;
}

std::string RequestHandler::getSize() const{

    return _size;
}


int RequestHandler::getNReadIndex() const{
    
    return _n_read_index;
}

/*
** ============================================================================
** Request Handler
** ============================================================================
*/

// build path toward file
std::string RequestHandler::getFile( std::string code ){
    
    std::string file = _root;
    file += "/";
    file += code;
    file += ".";
    file += "html";
 
    return file;
}
 
// construct message to send back to client 
//  header : code + Content-Type
std::string RequestHandler::buildAnswerHeader( std::string code, std::string type ){
    
    //cherche dans tableau >> code + reason
    std::string codeName[6] = {
        "index",
        "400",
        "404",
        "405",
        "414",
        "421"


    };
    
    int index = 0;
    for (int i = 0 ;i < 6; i++){
        
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
        str = "400 BAD REQUEST";
        break;

        case(2):
        str = "404 Not Found";
        break;

        case(3):
        str = "405 METHOD NOT ALLOWED";
        break;
        
        case(4):
        str = "414 URI TOO LONG";
        break;
        
        case(5):
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
    if ( type == "image/jpeg" ||  type == "image/png" ){
        
        _header += "\r\n";
        _header += "Context-Length: ";
        std::stringstream ss;
        ss << _n_read_index;
        std::string size = ss.str();
        _header += size;
        _header += "\r\n";
        _header += "Connection: keep-alive";

    }
    _header += "\r\n\r\n";
    return _header;
}

// open file + stock it in buffer to send back to client
// content = text
bool    RequestHandler::answerFile( std::string file ){

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
bool    RequestHandler::answerFileImage(){

    std::ifstream source("data/images/cat.png",std::ios::binary);

    // if (source.is_open() == false)
    // {
    //     std::cerr << "Error: file doesn't exist" << std::endl;
    //     return false;
    // }

    // if (source.peek() == std::ifstream::traits_type::eof())
    // {
    //     std::cerr << "Error: file is empty" << std::endl;
    //     return false;
    // }

    // source.seekg(0, std::ios::end);
    size_t size = source.tellg();
    // source.seekg(0, std::ios::beg);
    // char file_buffer[size];
    // source.read(file_buffer, size);
    
    // std::stringstream ss;
    // ss << size;
    // _size = ss.str();
    
    // std::cout << "Size: " << _size << std::endl;
    // if (size > BUF_SIZE){
    //     std::cerr << "Image size is too big." << std::endl;
    //     return false;
    // }

    int indexfd = open("data/images/cat.png", O_RDONLY);
    if (indexfd == -1){
        std::cerr << "Error file failed to open on indexfd:" << indexfd << std::endl;
        close(indexfd);
        return false;
    }
    this->_n_read_index = read(indexfd, _buffer, size);
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
    
    HTTPParser HTTPParser(_listTokens);

    //  check request
    if (HTTPParser.checkHost(listen) == false){
        std::cerr << "Error Host not found: " << strerror(errno) << std::endl;
    }
    
    // find method 
    else if (HTTPParser.findMethods() == false){
        std::cerr << "Error Method not implemented: " << strerror(errno) << std::endl;
    }

    if (HTTPParser.getType() == "text/html"){
        
        std::string file = getFile(HTTPParser.getCode()); 
        if (answerFile(file) == false)
            return false;
    }
    if (HTTPParser.getType() == "image/jpeg"){
        
        if (answerFileImage() == false)
            return false;
    }
    if (HTTPParser.getType() == "image/png"){
        
        if (answerFileImage() == false)
            return false;
    }
    
    buildAnswerHeader(HTTPParser.getCode(), HTTPParser.getType());

    

    return true;

}
