/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:20:07 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/28 15:32:34 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPParser.hpp"
#include "../lexer/Lexer.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

HTTPParser:: HTTPParser(  std::string const & request ) : _allTokens(), _request(request), _code(), _type(){
    
}

HTTPParser::HTTPParser( HTTPParser const & src ) : _allTokens(src._allTokens), _request(src._request), 
_code(src._code), _type(src._type){
    
}

HTTPParser::~HTTPParser(){
    
}

HTTPParser &    HTTPParser::operator=( HTTPParser const & other ){

    if (this != &other ){

        this->_allTokens = other._allTokens;
        this->_request = other._request;
        this->_code = other._code;
        this->_method = other._method;
        this->_type = other._type;

    }
    
    return *this;
}

std::string HTTPParser::getCode() const{
        
    return _code;
}

std::string HTTPParser::getType() const{
        
    return _type;
}

std::string HTTPParser::getMethod() const{
    
    return _method;
}

std::string HTTPParser::getRequestTarget() const{
    return _requesttarget;
}

std::string HTTPParser::setCode( std::string const & code ){
    
    _code = code;
    return _code;
}

std::string HTTPParser::setType( std::string const & type ){

    _type =type;
    return _type;
}


/*
** ============================================================================
** Lexer HTTP
** ============================================================================
*/

static std::string tokenTypeToString(TokenType type)
{
    switch(type)
    {
        case Word:      return "Word";
        case LBracket:  return "LBracket";
        case RBracket:  return "RBracket";
        case Semicolon: return "Semicolon";
        case Hashtag:   return "Hashtag";
        case End:       return "End";

        default:        return "Unknown";
    }
}


void print_token_chain(std::vector <Token> tokens)
{
    for (size_t i = 0; i < tokens.size(); i++)
    {
        std::cout
            << tokenTypeToString(tokens[i].type)
            << " => "
            << tokens[i].value
            << std::endl;
    }
    return ;
}

void HTTPParser::HTTPparse_file(const std::string &str)
{
    std::istringstream iss(str);
    std::string line;
    // std::vector<Token> allTokens;

    std::cout << "\n";
    while (std::getline(iss, line))
    {
        std::cout << line << "\n";
        Lexer lexer(line);
        std::vector<Token> lineTokens = lexer.tokenize();

        for (size_t i = 0; i < lineTokens.size(); i++)
        {
            if (lineTokens[i].type != End)
                _allTokens.push_back(lineTokens[i]);
        }
    }

    _allTokens.push_back(Token(End, ""));
    //print_token_chain(_allTokens);
    
}

/*
** ============================================================================
** Parser HTTP
** ============================================================================
*/

// CHECK REQUEST:
// -SIZE
// -METHOD
// -HOST

bool    HTTPParser::checkSize(){
    
    if (_allTokens.size() > BUF_SIZE){
        
        _code = "413";
        _type = "text/html";

        return false;
    }

    return true;

}

static bool isTokenWord( Token const & t ){
    
    return t.type == Word;
}


static bool isMethod( Token const & t ){
    
    return t.value == "GET" || t.value == "POST" || t.value == "DELETE";
}


// Check if it respect the standard form :
// request-line   = method SP request-target SP HTTP-version
bool    HTTPParser::checkRequestLine(){
    
    std::vector<Token>::iterator found;
    
    found = find_if(_allTokens.begin(), _allTokens.end(), isTokenWord);
    if (found != _allTokens.end()){ // same as EOF
        
        found = find_if(_allTokens.begin(), _allTokens.end(), isMethod);
        if (found != _allTokens.end()){
            
            _method = found->value;
            std::cout << "Method:" << _method << std::endl;
            found++;
            
            char const *slash = strrchr(found->value.c_str(), '/');
            if (slash){
                _requesttarget = found->value;
                std::cout << "RequestTarget: " << _requesttarget<< std::endl;
                found++;

                if (found->value == "HTTP/1.1"){
                
                    _httpversion = found->value;
                    std::cout << "HTTP version: " << _httpversion << std::endl;
                    return true;
                } 
            }

        }
        else{
            _code = "405";
            _type = "text/html";
            return false;
            
        }
    
    }

    _code = "400";
    _type = "text/html";
            
    return false;
}

// Check if the entry Host: correspond to the config file info
// or if it exist at all
bool    HTTPParser::checkHost( ListenerManager const & listener ){

    std::string hostname = listener.getNode();
    hostname += ":";
    hostname += listener.getService();
    
    std::vector<Token>::iterator it;

    for (it = _allTokens.begin(); it != _allTokens.end(); ++it)
    {
        if (it->type == Word)
        {
            if (it->value == "Host:")
            {
                it++;
                if (it->type == Word)
                {
                    if (it->value != hostname)
                    {
                        _code = "421";
                        _type = "text/html";

                        return false;
                    }
                }
            }
        }
        else if (it->type != Semicolon && it->type != End)
        {
            _code = "400";
            _type = "text/html";

            return false;
        }
    }
    return true;
}

bool    HTTPParser::isRequestValid( ListenerManager const & listen ){
    
    if (checkSize() == false){
        std::cerr << "Error Size too big: " << strerror(errno) << std::endl;
        return false;
    }
    
    if (checkRequestLine() == false){
        std::cerr << "Error Request Line wrong: " << strerror(errno) << std::endl;
        return false;
    }
    
    if (checkHost(listen) == false){
        std::cerr << "Error Host not found: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

bool    HTTPParser::findMethods(){
    
    if (_method == "GET"){
        
        if (_requesttarget == "/"){
            
            _code = "index";
            _type = "text/html";
            
            return true;
        }
        else if (_requesttarget == "/image.html"){
            
            _code = "image";
            _type = "text/html";
        }
        else if (_requesttarget.find("/images") != std::string::npos){
            
            std::cout <<  "found /images " << std::endl;
            
            char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
            if (lastPoint)
            std::cout <<  "lastPoint:" << lastPoint << std::endl;
            char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
            std::string name = std::string(lastSlash, strlen(lastSlash));
            name.erase(name.begin());

            std::cout <<  "name:" << name << std::endl;
            
            _code = name;
            std::string suffix = std::string(lastPoint, strlen(lastPoint));
            if (suffix  == ".jpg"){
                _type = "image/jpeg";
                return true;
            }
            if (suffix  == ".png"){
                _type = "image/png";
                return true;
            }
            if (suffix  == ".gif"){
                _type = "image/gif";
                return true;
            }
        }
        if (_requesttarget == "/favicon.ico"){
            
            _code = "favicon.ico";
            _type = "image/x-icon";
            
            return true;
        }
        if (_requesttarget.find("/upload") != std::string::npos){
            
            char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
            if (lastPoint)
            std::cout <<  "lastPoint:" << lastPoint << std::endl;
            
            char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
            std::string name = std::string(lastSlash, strlen(lastSlash));
            name.erase(name.begin());
            
            std::cout <<  "name:" << name << std::endl;
            
            _code = name; //? fichier specifique 
            std::string suffix = std::string(lastPoint, strlen(lastPoint));
            if (suffix == ".txt"){
                _type = "text/plain";
                return true;
            }
            if (suffix == ".html"){
                _type = "text/html";
                return true;
            }
        }
    }
    else if (_method == "POST"){
        
        _code = "200"; //? fichier specifique 
        return true;
    }
    else if (_method == "DELETE"){
        // check for CGI
        _code = "200"; //? fichier specifique 
        return true;
        
    }
    _code = "404";
    _type = "text/html";
    return false;
 
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
