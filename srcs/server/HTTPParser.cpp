/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:20:07 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/16 12:20:03 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPParser.hpp"
#include "../lexer/Lexer.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

HTTPParser:: HTTPParser( std::vector<Token> allTokens ) : _allTokens(allTokens), _code(){
    
}

HTTPParser::HTTPParser( HTTPParser const & src ) : _allTokens(src._allTokens), _code(src._code){
    
}

HTTPParser::~HTTPParser(){
    
}

HTTPParser &    HTTPParser::operator=( HTTPParser const & other ){

    if (this != &other ){

        this->_allTokens = other._allTokens;
        this->_code = other._code;
    }
    
    return *this;
}

std::string HTTPParser::getCode() const{
        
    return _code;
}

std::string HTTPParser::getType() const{
        
    return _type;
}

/*
** ============================================================================
** Parser HTTP
** ============================================================================
*/

// Check if the entry Host: correspond to the config file info
// or if it exist at all
bool    HTTPParser::checkHost( ListenerManager const & listener ){


    std::string hostname = listener.getNode();
    hostname += ":";
    hostname += listener.getService();
    
    std::vector<Token>::iterator it;

    for (it = _allTokens.begin(); it != _allTokens.end(); ++it){
    
        if (it->type == Word){
            
            if (it->value == "Host:"){
                it++;
                if (it->type == Word){
                    
                    if (it->value != hostname){
                        
                        _code = "421";
                        _type = "text/html";
                        return false;
                    }
                }
            }
        }
        else if (it->type != Semicolon && it->type != End) {
            
            _code = "400";
            _type = "text/html";

            return false;
        }
    }
    return true;
}

static bool isTokenWord( Token const & t ){
    
    return t.type == Word;
}

static bool isMethodGet( Token const & t ){
    
    return t.value == "GET";
}

static bool isMethodPost( Token const & t ){
    
    return t.value == "POST";
}

static bool isMethodDelete( Token const & t ){
    
    return t.value == "DELETE";
}

bool    HTTPParser::findMethods(){
    
    std::vector<Token>::iterator found;
    
    if (_allTokens.size() > BUF_SIZE){
        
        _code = "414";
        _type = "text/html";


        return false;
    }

  
    found = find_if(_allTokens.begin(), _allTokens.end(), isTokenWord);
    if (found != _allTokens.end()){ // same as EOF
        
        found = find_if(_allTokens.begin(), _allTokens.end(), isMethodGet);
        if (found != _allTokens.end()){
            
            found++;
           
            if (found->value == "/"){
                
                _code = "index";
                _type = "text/html";
                
                return true;
            }
            if (found->value.find("/images") != std::string::npos){
                
                std::cout <<  "found /images " << std::endl;

                char const *lastSlash = strrchr(found->value.c_str(), '.');
                if (lastSlash)
                    std::cout <<  "lastSlash:" << lastSlash << std::endl;
                std::string suffix = std::string(lastSlash);
                if (suffix  == ".jpg"){
                    
                    
                    _code = "index";
                    _type = "image/jpeg";
                    
                    return true;
                }
                if (suffix == ".png"){
                
                    
                    _code = "index";
                    _type = "image/png";
                    
                    return true;
                }
            }
            _code = "400";
            _type = "text/html";
            
            return true;
        }
        found = find_if(_allTokens.begin(), _allTokens.end(), isMethodPost);
        if (found != _allTokens.end()){
            
            _code = "200"; //? fichier specifique 
            return true;
        }
        found = find_if(_allTokens.begin(), _allTokens.end(), isMethodDelete);
        if (found != _allTokens.end()){
            
            _code = "200"; //? fichier specifique
            return true;
        }
    }
    else {
        _code = "405";
        _type = "text/html";

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


