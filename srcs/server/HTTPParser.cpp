/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:20:07 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/12 17:30:00 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "HTTPParser.hpp"
#include "../lexer/Lexer.hpp"
#include "../parser/config/ServerConfig.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/
  
HTTPParser::HTTPParser( std::string const & request, const ServerConfig &serverConfig ) :
_allTokens(), _request(request), _serverConfig(serverConfig), _code(), _type(), _method(),
_requesttarget(), _httpversion(), _boundary(), _fileLength(), _fileName(), _fileBuf(), _errors(false)
{
}

HTTPParser::HTTPParser( HTTPParser const & src ) :
_allTokens(src._allTokens), _request(src._request), _serverConfig(src._serverConfig),
_code(src._code), _type(src._type), _method(src._method), _requesttarget(src._requesttarget), _httpversion(src._httpversion),
  _boundary(src._boundary), _fileLength(src._fileLength), _fileName(src._fileName), _fileBuf(src._fileBuf), _errors(src._errors)
{
}

HTTPParser::~HTTPParser(){
    
}

HTTPParser &    HTTPParser::operator=( HTTPParser const & other ){

    if (this != &other ){

        this->_request = other._request;
        this->_code = other._code;
        this->_type = other._type;
        this->_method = other._method;
        this->_requesttarget = other._requesttarget;
        this->_httpversion = other._httpversion;
        this->_boundary = other._boundary;
        this->_fileLength = other._fileLength;
        this->_fileName = other._fileName;
        this->_fileBuf = other._fileBuf;
        this->_errors = other._errors;
        
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

bool     HTTPParser::getError() const{
    
    return _errors;
}

std::string HTTPParser::getRequestTarget() const{

    return _requesttarget;
}

std::string HTTPParser::setCode( std::string const & code ){
    
    _code = code;
    return _code;
}

std::string HTTPParser::setType( std::string const & type ){

    _type = type;
    return _type;
}

bool HTTPParser::setError(bool error){
    
    _errors = error;
    return _errors;
}



/*
** ============================================================================
** Lexer HTTP
** ============================================================================
*/

// static std::string tokenTypeToString(TokenType type)
// {
//     switch(type)
//     {
//         case Word:      return "Word";
//         case LBracket:  return "LBracket";
//         case RBracket:  return "RBracket";
//         case Semicolon: return "Semicolon";
//         case Hashtag:   return "Hashtag";
//         case End:       return "End";

//         default:        return "Unknown";
//     }
// }


// void print_token_chain(std::vector <Token> tokens)
// {
//     for (size_t i = 0; i < tokens.size(); i++)
//     {
//         std::cout
//             << tokenTypeToString(tokens[i].type)
//             << " => "
//             << tokens[i].value
//             << std::endl;
//     }
//     return ;
// }

// void HTTPParser::HTTPparse_file(const std::string &str)
// {
//     std::istringstream iss(str);
//     std::string line;
//     // std::vector<Token> allTokens;

//     std::cout << "\n";
//     while (std::getline(iss, line))
//     {
//         std::cout << line << "\n";
//         Lexer lexer(line);
//         std::vector<Token> lineTokens = lexer.tokenize();

//         for (size_t i = 0; i < lineTokens.size(); i++)
//         {
//             if (lineTokens[i].type != End)
//                 _request.push_back(lineTokens[i]);
//         }
//     }

//     _request.push_back(Token(End, ""));
//     //print_token_chain(_request);
    
// }

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
// New Logic Parser Without Token :
// Look for spaces +store them in vector<size_t> : collectSpace()
// Then look at the strings in between those spaces : loop on vector<> space_inter : collectString()
// Assign string to variables after checks 

bool    HTTPParser::checkSize(){
    
    if (_request.size() > BUF_SIZE){
        
        _errors = true;
        _code = "413";
        _type = "text/html";

        return false;
    }

    return true;
}

// static bool isTokenWord( Token const & t ){
    
//     return t.type == Word;
// }

static bool isMethod( std::string const & str ){
    
    return str == "GET" || str == "POST" || str == "DELETE";
}

// static bool isHost( std::string const & t ){
    
//     return t == "Host:";
// }

// static bool isContentType( std::string const & t ){
    
//     return t == "Content-Type:";
// }

// static bool isContentLength( std::string const & t ){
    
//     return t == "Content-Length:";
// }

// static bool isContentDisposition( std::string const & t ){
    
//     return t == "Content-Disposition:";
// }

// static bool isDelimiter( std::string const & t ){
    
//     return t == "\r\n\r\n";
// }

static bool isSimpleSpace( int found ){
    
    return std::isspace(static_cast<unsigned char>(found));
}

// static bool isBoundary( Token const & t, std::string const & boundary ){
    
//     return t.value == boundary;
// }


std::vector<size_t>  HTTPParser::collectSpace( std::string::iterator pos ){
    
    std::vector<size_t> space_inter;
    
    while(pos != _request.end()){
        
        pos = find_if(pos, _request.end(), isSimpleSpace);
        if (pos == _request.end())
            break;
            
        space_inter.push_back(distance(_request.begin(), pos));
        if (*pos == '\n')
            break;

        pos++;
    }
    
    _pos = pos;

    return space_inter;
}

std::vector<std::string>  HTTPParser::collectString(  std::vector<size_t> space_inter ){

    std::vector<std::string> subss;
    
    for(size_t i = 0; i < space_inter.size(); ++i){
        
        size_t start = (i == 0) ? 0 : space_inter[i - 1] + 1;
        size_t end = space_inter[i];

        subss.push_back(_request.substr(start, end - start));
    }

    return subss;
}


// Check if it respect the standard form :
// request-line   = method SP request-target SP HTTP-version
bool    HTTPParser::checkRequestLine(){
    
    std::string::iterator space = _request.begin();
    
    std::vector<size_t> space_inter = collectSpace(space);

    std::vector<std::string> subss = collectString(space_inter);
 
    if (!subss.empty()){
        
        _method = subss[0];
        
        if (isMethod(_method))
        std::cout << "Method:" << _method << std::endl;
        else{
            
            _errors = true;
            _code = "405";
            _type = "text/html";
            
            return false;
        }
        
        _requesttarget = subss[1];
        
        char const *slash = strrchr( _requesttarget.c_str(), '/');
        if (slash)
        std::cout << "RequestTarget: " << _requesttarget << std::endl;
        
        _httpversion = subss[2];
        if ( _httpversion == "HTTP/1.1")
        std::cout << "HTTP version: " << _httpversion << std::endl;
     
        return true;
    }
    // if (found != _request.end()){ // same as EOF
        
    //     found = find_if(_request.begin(), _request.end(), isMethod);
    //     if (found != _request.end()){
            
    //         _method = found->value;
    //         std::cout << "Method:" << _method << std::endl;
    //         found++;
            
    //         char const *slash = strrchr(found->value.c_str(), '/');
    //         if (slash){
          
    //             _requesttarget = found->value;
    //             std::cout << "RequestTarget: " << _requesttarget<< std::endl;
    //             found++;
                
    //             if (found->value == "HTTP/1.1"){
                
    //                 _httpversion = found->value;
    //                 std::cout << "HTTP version: " << _httpversion << std::endl;
    //                 return true;
    //             } 
    //         }
    //     }
    //     else{
            
    //         _code = "405";
    //         _type = "text/html";
    //         return false;
    //     }
    // }

    _errors = true;
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

    _pos++;
   
    std::vector<size_t> space_inter = collectSpace(_pos);

    std::vector<std::string> subss = collectString(space_inter);

    if (!subss.empty()){
            
        if (subss[1] != hostname){
            
            std::cout << "found->value:" << subss[1] << std::endl;
            _errors = true;
            _code = "421";
            _type = "text/html";
            
            return false;
        }
        
        return true;
    }
    
    // found = find_if(_request.begin(), _request.end(), isTokenWord);
    // if (found != _request.end()){ // same as EOF
        
    //     found = find_if(_request.begin(), _request.end(), isHost);
    //     if (found != _request.end()){
            
    //         found++;
    //         if (found->value != hostname){
                
    //             std::cout << "found->value:" << found->value << std::endl;

    //             _code = "421";
    //             _type = "text/html";
                
    //             return false;
    //         }
            
    //         return true;
            
    //     }
    // }
    
    _errors = true;
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

    size_t start = _request.find("Content-Type:"); 
    std::string::iterator space = _request.begin() + start;
    
    std::vector<size_t> space_inter = collectSpace(space);

    std::vector<std::string> subss = collectString(space_inter);

    if (!subss.empty()){
        
        _type = subss[1];
        _type.erase(_type.end() - 1);
        std::cout << "Content-Type:" << _type << std::endl;
        _boundary = subss[2];
        _boundary.erase(_boundary.begin(),_boundary.begin()+9);
        
        std::cout << "boundary:" << _boundary << std::endl;
        
        return true;
    }
    // std::vector<Token>::iterator found;
    
    // found = find_if(_request.begin(), _request.end(), isTokenWord);
    // if (found != _request.end()){ // same as EOF
        
        
    //     found = find_if(_request.begin(), _request.end(), isContentType);
    //     if (found != _request.end()){
            
    //         found++;
    //         _type = found->value;
        
    //         std::cout << "Content-Type:" << _type << std::endl;
    //         found++;
    //         found++;
    //         _boundary = found->value;
    //         _boundary.erase(_boundary.begin(),_boundary.begin()+9);
    //         std::cout << "boundary:" << _boundary << std::endl;

    //         return true;
    //     }
    // }
    _errors = true;
    _code = "400";
    _type = "text/html";
    
    return false;
}


bool    HTTPParser::checkContentLength(){

    _pos++;
    std::string::iterator space = _pos;
    
    std::vector<size_t> space_inter = collectSpace(space);

    std::vector<std::string> subss = collectString(space_inter);
 
     if (!subss.empty()){
        
        _fileLength = subss[1];
        std::stringstream ss(_fileLength);
        size_t len; 
        ss >> len;
        std::cout << "Content-Length:" << _fileLength << std::endl;
        std::cout << "Content-Length:" << len << std::endl;
        if (len > BUF_SIZE){
            std::cerr << "File size is too big." << std::endl;
            return false;
        }
        return true;
     }
    // std::vector<Token>::iterator found;
    
    // found = find_if(_request.begin(), _request.end(), isTokenWord);
    // if (found != _request.end()){ // same as EOF
        
    //     found = find_if(_request.begin(), _request.end(), isContentLength);
    //     if (found != _request.end()){
            
    //         found++;
    //         _fileLength = found->value;
    //         std::stringstream ss(_fileLength);
    //         size_t len; 
    //         ss >> len;
    //         std::cout << "Content-Length:" << _fileLength << std::endl;
    //         std::cout << "Content-Length:" << len << std::endl;
    //         while(found->type != LBracket){
    //             if (found->value != _boundary){
                    
    //                 std::cout << "found->value:" << found->value << std::endl;
    //                 found++;
    //             }
    //         }

    //         if (len > BUF_SIZE){
    //             std::cerr << "File size is too big." << std::endl;
    //             return false;
    //         }

    //         return true;
    //     }
 

    _errors = true;
    _code = "400";
    _type = "text/html";
    
    return false;
}


bool    HTTPParser::checkContentDisposition(){
    
    size_t pos = _request.find("Content-Disposition:");
    
    std::string::iterator newpos = _request.begin() + pos;

    std::vector<size_t> space_inter = collectSpace(newpos);


    std::vector<std::string> subss = collectString(space_inter);
    // size_t end = _request.find('\n');

    // std::string contentdisposition = _request.substr(pos, end - pos);
    // std::cout << "contentdisposition: " << contentdisposition << std::endl;
     
     if (!subss.empty()){
        
            std::string type = subss[1];
            char const *slash = strchr(_type.c_str(), '/');
            std::string checktype = std::string(slash, strlen(slash));
            checktype.erase(checktype.begin());
            checktype.erase(checktype.end() - 1);
            // if (type != checktype)
            //     return false;
            std::string name = subss[2];
            name.erase(name.end() - 1);
            name.erase(name.begin(), name.begin() + 6);
            _fileName = subss[3];
            _fileName.erase(_fileName.end() - 1);
            _fileName.erase(_fileName.begin(), _fileName.begin() + 10 );
            std::cout << "type: " << checktype << std::endl; //recuperer ??
            std::cout << "name " << name << std::endl;
            std::cout << "_fileName: " << _fileName << std::endl;

            return true;
     }



    
    // std::vector<Token>::iterator found;
    
    // found = find_if(_request.begin(), _request.end(), isTokenWord);
    // if (found != _request.end()){ // same as EOF
        
    //     found = find_if(_request.begin(), _request.end(), isContentDisposition);
    //     if (found != _request.end()){
            
    //         found++;
    //         std::string type = found->value;
    //         char const *slash = strchr(_type.c_str(), '/');
    //         std::string checktype = std::string(slash, strlen(slash));
    //         checktype.erase(checktype.begin());
    //         if (type != checktype)
    //             return false;
    //         found++;
    //         found++;
    //         char const *equal = strchr(found->value.c_str(), '"');
    //         std::string name = std::string(equal, strlen(equal));
    //         name.erase(name.end() - 1);
    //         name.erase(name.begin());
    //         found++;
    //         found++;
    //         char const *quote = strchr(found->value.c_str(), '"');
    //         _fileName = std::string(quote, strlen(quote));
    //         _fileName.erase(_fileName.end() - 1);
    //         _fileName.erase(_fileName.begin());
    //         std::cout << "_fileName:" << _fileName<< std::endl;
    //         _found = found;

    //         return true;
    //     }
    // }

    _errors = true;
    _code = "400";
    _type = "text/html";
    
    return false;
}


bool    HTTPParser::gatherFile(){
    
    _pos++;
    _pos++;
    _pos++;
    std::string endOfFile = _boundary + "--";

    size_t len =  endOfFile.size();
    
    while (_pos != _request.end()){

        // if (_pos != endOfFile)
            _fileBuf += *_pos;
        _pos++;
    }

    _fileBuf.erase(_fileBuf.end() - (len + 2), _fileBuf.end());
    return true;
    
    // _code = "400";
    // _type = "text/html";
    
    // return false;
}

std::string     HTTPParser::addSuffix(std::string suffix){
    
    if (suffix  == ".jpg")
        _type = "image/jpeg";
    if (suffix  == ".png")
        _type = "image/png";
    if (suffix  == ".gif")
        _type = "image/gif";
    if (suffix  == ".webp")
        _type = "image/webp";
    if (suffix == ".txt")
        _type = "text/plain";
    if (suffix == ".html")
        _type = "text/html";
    return _type;
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
        else if (_requesttarget == "/gallery.html" || _requesttarget == "/html/gallery.html" || _requesttarget == "/html/html/gallery.html"){
            
            _code = "gallery";
            _type = "text/html";
            return true;

        }
        else if (_requesttarget.find("/images") != std::string::npos){
            
            char const *lastPoint = strrchr(_requesttarget.c_str(), '.');
            char const *lastSlash = strrchr(_requesttarget.c_str(), '/');
            std::string name = std::string(lastSlash, strlen(lastSlash));
            name.erase(name.begin());

            _code = name;
            std::string suffix = std::string(lastPoint, strlen(lastPoint));
            _type = addSuffix(suffix);
            
            return true;
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
        _code = "index";
        _type = "text/html";
        return true;
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
                _type = addSuffix(suffix);
                return true;
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
            // _code = "200"; //? fichier specifique 
            // 204 No Content
            _type = addSuffix(suffix);
            return true;
        }
    }
    _errors = true;
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