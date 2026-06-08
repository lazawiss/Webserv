/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:20:07 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/08 16:43:20 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPParser.hpp"
#include "../lexer/Lexer.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

HTTPParser:: HTTPParser( std::vector<Token> allTokens ) : _allTokens(allTokens), 
_root("data/html"), _index("index.html"), _error("404.html"), _header(), _buffer(""), _n_read_index(0){
    
}

HTTPParser::HTTPParser( HTTPParser const & src ) : _allTokens(src._allTokens), 
_root(src._root), _index(src._index), _error(src._error), _header(src._header), _n_read_index(src._n_read_index){
    
     _buffer[BUF_SIZE] = src._buffer[BUF_SIZE];
}

HTTPParser::~HTTPParser(){
    
}

HTTPParser &    HTTPParser::operator=( HTTPParser const & other ){

    if (this != &other ){

        this->_allTokens = other._allTokens;
        this->_root = other._root;
        this->_index = other._index;
        this->_error = other._error;
        this->_header = other._header;
        this->_buffer[BUF_SIZE] = other._buffer[BUF_SIZE];
        this->_n_read_index = other._n_read_index;
    }
    
    return *this;
}


std::string HTTPParser::getBuffer() const{

    return _buffer;
}

std::string HTTPParser::getHeader() const{

    return _header;
}

int HTTPParser::getNReadIndex() const{
    
    return _n_read_index;
}

/*
** ============================================================================
** Parser HTTP
** ============================================================================
*/

static bool isTokenWord( Token const & t ){
    
    return t.type == Word;
}

bool    HTTPParser::findMethods(){
    
    std::vector<Token>::iterator found;
    
    // found = find(_allTokens.begin(), _allTokens.end(),"WORD");
    if (_allTokens.size() > BUF_SIZE){
        
        std::string file = getFile414(); 
        if (answerFile(file) == false)
            return false;
    }
    
    found = find_if(_allTokens.begin(), _allTokens.end(), isTokenWord);
    if (found != _allTokens.end()){ // same as EOF
        
        buildAnswerHeader();
        std::string file = getFile200(); 
        if (answerFile(file) == false)
            return false;
    }
    
    // if (_request.find("GET / HTTP/1.1") != std::string::npos){ // same as EOF
        
    //     std::string file = getFile200(); 
    //     if (answerFile(file) == false)
    //         return false;
    // }
    
    // if (_request.find("GET /images/trees.jpg HTTP/1.1") != std::string::npos){
        
      
    // }
    
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

std::string HTTPParser::getFile( std::string fileName ){
    
    std::string file = _root;
    file += "/";
    file += fileName;
 
    return file;
}

std::string HTTPParser::getFile200(){
     
    std::string file = _root;
    file += "/";
    file += _index;
 
    return file;
}

std::string HTTPParser::getFile404(){

    std::string file = _root;
    file += "/";
    file += _error;
  
    return file;
}

std::string HTTPParser::getFile414(){

    std::string file = _root;
    file += "/";
    file += "414.html";
  
    return file;
}

std::string HTTPParser::buildAnswerHeader(){
    
    _header = "HTTP/1.1 200 OK\r\n";
    _header += "Content-Type: text/html\r\n\r\n"; 

    return _header;
}


bool    HTTPParser::answerFile( std::string file ){

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
