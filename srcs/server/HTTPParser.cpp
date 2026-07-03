/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:20:07 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/03 21:35:05 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPParser.hpp"
#include "../lexer/Lexer.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

HTTPParser:: HTTPParser(  std::string const & request ) : _allTokens(), _request(request),
 _code(), _type(), _method(), _requesttarget(), _httpversion(), _boundary(), _fileName(), _fileBuf(){
    
}

HTTPParser::HTTPParser( HTTPParser const & src ) : _allTokens(src._allTokens), _request(src._request), 
_code(src._code), _type(src._type), _method(src._method), _requesttarget(src._requesttarget),
 _httpversion(src._httpversion), _boundary(src._boundary), _fileName(src._fileName), _fileBuf(src._fileBuf){
    
}

HTTPParser::~HTTPParser(){
    
}

HTTPParser &    HTTPParser::operator=( HTTPParser const & other ){

    if (this != &other ){

        this->_allTokens = other._allTokens;
        this->_request = other._request;
        this->_code = other._code;
        this->_type = other._type;
        this->_method = other._method;
        this->_requesttarget = other._requesttarget;
        this->_httpversion = other._httpversion;
        this->_boundary = other._boundary;
        this->_fileName = other._fileName;
        this->_fileBuf =other._fileBuf;
        

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

std::string HTTPParser::getBoundary() const{
        
    return _boundary;
}

std::string HTTPParser::getFileName() const{
        
    return _fileName;
}

std::string HTTPParser::getFileBuf() const{
        
    return _fileBuf;
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
** Parser HTTP - First Part: GET
** ============================================================================
*/

// CHECK REQUEST:
// -SIZE
// -METHOD
// -HOST
// making sure the request have the minimum requirement to execute the static part with GET 

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

static bool isHost( Token const & t ){
    
    return t.value == "Host:";
}

static bool isContentType( Token const & t ){
    
    return t.value == "Content-Type:";
}

static bool isContentLength( Token const & t ){
    
    return t.value == "Content-Length:";
}

static bool isContentDisposition( Token const & t ){
    
    return t.value == "Content-Disposition:";
}

// static bool isBoundary( Token const & t, std::string const & boundary ){
    
//     return t.value == boundary;
// }


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

    std::cout << "hostname:" << hostname << std::endl;
        std::vector<Token>::iterator found;
    
    found = find_if(_allTokens.begin(), _allTokens.end(), isTokenWord);
    if (found != _allTokens.end()){ // same as EOF
        
        
        found = find_if(_allTokens.begin(), _allTokens.end(), isHost);
        if (found != _allTokens.end()){
            
            found++;
            if (found->value != hostname){
                
                std::cout << "found->value:" << found->value << std::endl;

                _code = "421";
                _type = "text/html";
                
                return false;
            }
            
            return true;
            
        }
    }
    _code = "400";
    _type = "text/html";
    
    return false;
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

/*
** ============================================================================
** Parser HTTP - Second Part: POST
** ============================================================================
*/

//Parsing the minimum requirement to upload file with POST:
// - CONTENT-TYPE
// - CONTENT-LENGTH
// - CONTENT-DISPOSITION :
//      - TYPE : ex: form-data
//      - NAME : part of the form that collects uploaded file
//      - FILENAME : NAME OF THE UPLOADED FILE

bool    HTTPParser::checkContentType(){

        std::vector<Token>::iterator found;
    
    found = find_if(_allTokens.begin(), _allTokens.end(), isTokenWord);
    if (found != _allTokens.end()){ // same as EOF
        
        
        found = find_if(_allTokens.begin(), _allTokens.end(), isContentType);
        if (found != _allTokens.end()){
            
            found++;
            _type = found->value;
        
            std::cout << "Content-Type:" << _type << std::endl;
            found++;
            found++;
            _boundary = found->value;
            _boundary.erase(_boundary.begin(),_boundary.begin()+9);
            std::cout << "boundary:" << _boundary << std::endl;

            return true;
        }
    }
    _code = "400";
    _type = "text/html";
    
    return false;
}

bool    HTTPParser::checkContentLength(){

    std::vector<Token>::iterator found;
    
    found = find_if(_allTokens.begin(), _allTokens.end(), isTokenWord);
    if (found != _allTokens.end()){ // same as EOF
        
        found = find_if(_allTokens.begin(), _allTokens.end(), isContentLength);
        if (found != _allTokens.end()){
            
            found++;
            _fileLength = found->value;
            std::stringstream ss(_fileLength);
            size_t len; 
            ss >> len;
            std::cout << "Content-Length:" << _fileLength << std::endl;
            std::cout << "Content-Length:" << len << std::endl;
            while(found->type != LBracket){
                if (found->value != _boundary){
                    
                    std::cout << "found->value:" << found->value << std::endl;
                    found++;
                }
            }

            if (len > BUF_SIZE){
                std::cerr << "File size is too big." << std::endl;
                return false;
            }

            return true;
        }
    }
    _code = "400";
    _type = "text/html";
    
    return false;
}


bool    HTTPParser::checkContentDisposition(){
    
    std::vector<Token>::iterator found;
    
    found = find_if(_allTokens.begin(), _allTokens.end(), isTokenWord);
    if (found != _allTokens.end()){ // same as EOF
        
        found = find_if(_allTokens.begin(), _allTokens.end(), isContentDisposition);
        if (found != _allTokens.end()){
            
            found++;
            std::string type = found->value;
            char const *slash = strchr(_type.c_str(), '/');
            std::string checktype = std::string(slash, strlen(slash));
            checktype.erase(checktype.begin());
            if (type != checktype)
                return false;
            found++;
            found++;
            char const *equal = strchr(found->value.c_str(), '"');
            std::string name = std::string(equal, strlen(equal));
            name.erase(name.end() - 1);
            name.erase(name.begin());
            found++;
            found++;
            char const *quote = strchr(found->value.c_str(), '"');
            _fileName = std::string(quote, strlen(quote));
            _fileName.erase(_fileName.end() - 1);
            _fileName.erase(_fileName.begin());
            std::cout << "_fileName:" << _fileName<< std::endl;
            _found = found;

            return true;
        }
    }
    _code = "400";
    _type = "text/html";
    
    return false;
}


bool    HTTPParser::gatherFile(){
    
    _found++;
    _found++;
    _found++;
    std::string endOfFile = _boundary + "--";

    size_t len =  endOfFile.size();
    
    while (_found->type != End){

        if (_found->value != endOfFile)
            _fileBuf += _found->value;
        _found++;
    }

    _fileBuf.erase(_fileBuf.end() - (len + 2), _fileBuf.end());
    return true;
    
    // _code = "400";
    // _type = "text/html";
    
    // return false;
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
            return true;

        }
        else if (_requesttarget == "/html/gallery.html"){
            
            _code = "gallery";
            _type = "text/html";
            return true;

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
            
            _code = "upload";
            _type = "text/html";
            return true;
        }
    }
    else if (_method == "POST"){
        if (_requesttarget.find("/upload") != std::string::npos){
            
            if (_requesttarget == "/upload"){
                
                if (checkContentType() == false){
                    std::cerr << "Error Content-Type not found: " << strerror(errno) << std::endl;
                    return false;
                }
                if (checkContentLength() == false){
                    std::cerr << "Error Content-Length not found: " << strerror(errno) << std::endl;
                    return false;
                }
                if (checkContentDisposition() == false){
                    std::cerr << "Error Content-Disposition not found: " << strerror(errno) << std::endl;
                    return false;
                }
                if (gatherFile() == false){
                    std::cerr << "Error Content not found: " << strerror(errno) << std::endl;
                    return false;
                }
                return true;
            }
            else{
                
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
            //on success send 201 CREATED + Location: path to ressource
            // return true;
        }
    }
    else if (_method == "DELETE"){
        
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
            
            // _code = "200"; //? fichier specifique 
            // 204 No Content
            return true;
        }
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
