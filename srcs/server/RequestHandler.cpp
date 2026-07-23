/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andikim <andikim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 13:03:45 by lzannis           #+#    #+#             */
/*   Updated: 2026/07/23 09:30:22 by andikim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
#include "HTTPParser.hpp"
#include "RequestHandler.hpp"
#include "../parser/config/ServerConfig.hpp"
#include <dirent.h> 

/*
** ============================================================================
** Constructors & Destructor & Getters
** ============================================================================
*/


RequestHandler::RequestHandler( std::string const & request, const ServerConfig &serverConfig ) :
    _request(request), _serverConfig(serverConfig),
    _root(serverConfig.getRoot()), _header(), _size(), _pathToFile(), _n_read_index(0), _isCGI(false),
    _fullPath(), _query_string(), _scriptFilename(), _body(), _content_type(), _content_length(),
    _method()
{
    std::cout << "[RequestHandler] _root: '" << _root << "'" << std::endl;
    memset(_buffer, 0, BUF_SIZE);
}

RequestHandler::RequestHandler( RequestHandler const & src ) :
    _request(src._request), _serverConfig(src._serverConfig),
    _root(src._root), _header(src._header),
    _size(src._size), _pathToFile(src._pathToFile), _n_read_index(src._n_read_index), _isCGI(src._isCGI),
    _fullPath(src._fullPath), _query_string(src._query_string), _scriptFilename(src._scriptFilename), 
    _body(src._body), _content_type(src._content_type), _content_length(src._content_length),
    _method(src._method)
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
        this->_pathToFile = other._pathToFile;
        memcpy(this->_buffer, other._buffer, BUF_SIZE);
        this->_n_read_index = other._n_read_index;
        this->_isCGI = other._isCGI;
        this->_fullPath = other._fullPath;
        this->_query_string = other._query_string;
        this->_scriptFilename = other._scriptFilename;
        this->_body = other._body;
        this->_content_type = other._content_type;
        this->_content_length = other._content_length;
        this->_method = other._method;
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

bool RequestHandler::getCGI() const {
    return _isCGI;
}

/*
** ============================================================================
** Request Handler
** ============================================================================
*/

static std::string resolveRoot(const ServerConfig &cfg, const std::string &uri)
{
    const std::vector<LocationConfig> &locs = cfg.getLocations();
    size_t bestLen = 0;
    std::string root;
    for (size_t i = 0; i < locs.size(); ++i)
    {
        const std::string &path = locs[i].getPath();
        if (uri.find(path) == 0 && path.size() > bestLen)
        {
            bestLen = path.size();
            root = locs[i].getRoot();
        }
    }
    return root;
}

// build path toward file
std::string RequestHandler::getFile( std::string const & code, bool const & error ){
    std::string file;
    if (error == true){
        
        file = "data/errors/";
        file += code;
        file += ".";
        file += "html";
    }
    else {
        
        file = _root;
        file += "/";
        file += code;
    }
 
    std::cout << "File:" << file << std::endl;
    return file;
}

// build path toward file
std::string RequestHandler::getFileImage( std::string const & code){
    
    std::string file = "data/www/images";
    file += "/";
    file += code;
    std::cout << "FileImage:" << file << std::endl;
    return file;
}
// build path toward file
std::string RequestHandler::getFileUpload( std::string const & code){
    
    std::string file = "data/upload";
    file += "/";
    file += code;
    std::cout << "FileImage:" << file << std::endl;
    return file;
}

std::string RequestHandler::getPath() const{
        return _fullPath; // location root + script name, set in handleRequest
    }

std::string     RequestHandler::getFilename() const {
        return _scriptFilename;
    }

std::string     RequestHandler::getQueryString() const {
        return _query_string;
    }
std::string     RequestHandler::getBody() const{
        return _body;
    }
std::string     RequestHandler::getContentType() const{
        return _content_type;
    }
std::string     RequestHandler::getContentLength() const{
        return _content_length;
    }
std::string     RequestHandler::getMethod() const {
        return _method;
    }
 
// construct message to send back to client 
//  header : code + Content-Type
std::string RequestHandler::buildAnswerHeader( std::string const & code, std::string const & type ){
    
    std::cout << "code:" << code << std::endl;
    
    //cherche dans tableau >> code + reason
    std::string codeName[8] = {
    
        "400",
        "404",
        "405",
        "413",
        "414",
        "421",
        "201",
        "204"
    };
    
    int index = -1;
    for (int i = 0 ;i < 8; i++){
        
        if (codeName[i] == code){
            index = i;
            break;
        }
    }
    
    std::string str;

    switch (index) {
        
        case(0):
        str = "400 BAD REQUEST";
        break;

        case(1):
        str = "404 Not Found";
        break;

        case(2):
        str = "405 METHOD NOT ALLOWED\r\nAllow: GET, POST, DELETE";
        break;
        
        case(3):
        str = "413 CONTENT TOO LARGE";
        break;
        
        case(4):
        str = "414 URI TOO LONG";
        break;
        
        case(5):
        str = "421 MISDIRECTED REQUEST";
        break;
        
        case(6):
        str = "201 CREATED\r\nLocation: " + _pathToFile;
        break;
        
         case(7):
        str = "204 NO CONTENT";
        break;

        default:
        str = "200 OK";
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
    if (indexfd == -1)
    {
        std::cerr << "Error file failed to open on indexfd:" << indexfd << std::endl;
        return false;
    }
    this->_n_read_index = read(indexfd, _buffer, BUF_SIZE);
    close(indexfd);
    std::cout << "n_read_index:" << _n_read_index << std::endl;
    if (_n_read_index == -1)
        return false;
    if (_n_read_index > BUF_SIZE){
        std::cerr << "File size is too big." << std::endl;
        return false;
    }
    
    return true;
    
}


// open file + stock it in buffer to send back to client
// content = x-icon
bool    RequestHandler::answerFileIcon(){

    struct stat sb;
    
    if (stat("data/www/favicon.ico/favicon-16x16.png", &sb) == -1){
        std::cerr << "Error stat: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "File Size : " << sb.st_size <<std::endl;

    int indexfd = open("data/www/favicon.ico/favicon-16x16.png", O_RDONLY);
    if (indexfd == -1){
        std::cerr << "Error file failed to open on indexfd:" << indexfd << std::endl;
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


bool    RequestHandler::uploadFile( std::string const & filename, std::string const & buf ){
    
    _pathToFile = "data/upload/" + filename;
    
    std::cout << "_pathToFile: " << _pathToFile <<std::endl;
    
    std::ofstream outfile(_pathToFile.c_str(), std::ios::binary);
    if (!outfile){
        std::cerr << "Error outfile couldn't open: " << strerror(errno) << std::endl;
        return false;
    } 
    outfile.write(buf.data(), buf.size());

    if (!outfile.good()){
        std::cerr << "Error fail to write in file: " << strerror(errno) << std::endl;
        return false;
    }
    _n_read_index = buf.size();
    
    outfile.close();

    
    struct stat sb;
    
    if (stat(_pathToFile.c_str(), &sb) == -1){
        std::cerr << "Error stat outfile: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "File Size OUtfile: " << sb.st_size <<std::endl;
    

    if (_pathToFile.empty()){
        std::cerr << "Error outfile is empty: " << strerror(errno) << std::endl;
        return false;
    }
    
    
    return true;
}

bool    RequestHandler::removeFile( std::string const & filename ){
    
    _pathToFile = filename;
    
    std::cout << "_pathToFile: " << _pathToFile <<std::endl;
    
    struct stat sb;
    
    if (stat(_pathToFile.c_str(), &sb) == -1){
        std::cerr << "Error stat outfile: " << strerror(errno) << std::endl;
        return false;
    }
    std::cout << "File Size OUtfile: " << sb.st_size <<std::endl;

    if (remove(_pathToFile.c_str()) < 0 ){
        std::cerr << "Error file could not get deleted: " << strerror(errno) << std::endl;
        return false;
    }
    
    return true;
}

std::string RequestHandler::generateAutoindex(const std::string &fullPath, const std::string &requestTarget)
{
    DIR *dir = opendir(fullPath.c_str());
    if (dir == NULL)
        return ("");
    // and then after should be 403? or 500, is it an error unexpected though?
    std::string html;
    html += "<!DOCTYPE html>\n<html>\n<head><title>Index of ";
    html+= requestTarget; // or requestTarget - is it same thing here?
    html += "</title></head>\n<body>\n<h1>Index of ";
    html += requestTarget;
    html += "</h1>\n<hr>\n<ul>\n";

    struct dirent *entry; // format of directory entries, useful to grab all the files that are existing, girl
    while ((entry = readdir(dir)) != NULL)
    {
        std::string name = entry->d_name;
        if (name == ".")
            continue;
        std::string entryPath = fullPath;  // check entry a dir
        if (entryPath[entryPath.size() - 1] != '/')
            entryPath += "/";
        entryPath += name;
        
        struct stat entry_stat;
        bool isDir = (stat(entryPath.c_str(), &entry_stat) == 0 
            && S_ISDIR(entry_stat.st_mode));
        html = html + "<li><a href=\"";
        html += name;
        if (isDir)
            html += "/"; // this = trailing slash on directory
        html += "\">";
        html += name;
        if (isDir)
            html += "/";
        html += "</a></li>\n";
    }
    closedir(dir);

    html += "</ul>\n<hr>\n</body>\n</html>\n";
    return html;
}

void    RequestHandler::sendError( HTTPParser & parser ){
    
    parser.setError(true);
    parser.setCode("404");
    parser.setType("text/html");
    std::string file = getFile(parser.getCode(), parser.getError()); 
    answerFile(file);
}

// main function : 
// instanciate HTTPParser 
// checks if request valid
// parse request
// find proper file to send to client
// build answer depending of content to sent
bool    RequestHandler::handleRequest(  ListenerManager const & listen ){
    
    HTTPParser HTTPParser(_request, _serverConfig);

    bool    requestValid = true;

    //  check request
    if (HTTPParser.isRequestValid(listen) == false){
        std::cout << "Request Invalid." << std::endl;
        requestValid = false;
    }

    else if (requestValid == true)
    {
        if (requestValid == true && HTTPParser.isCGI())
        {
            if (HTTPParser.validateCGIRequest() == false){
                std::cout << "CGI Request not validated" << std::endl;
                return false;
            }
            _scriptFilename = HTTPParser.getScriptFilename();
            _fullPath = "data/" + _scriptFilename; // location.root + _scriptFilename
            _query_string = HTTPParser.getQueryString();
            _body = HTTPParser.getBody(); // need to parse still
            _content_type = HTTPParser.getContentType();
            _content_length = HTTPParser.getContentLength();
            _method = HTTPParser.getMethod();      
            _isCGI = true;
            return true;
        }
        else
            std::cerr << "Error Method not implemented: " << strerror(errno) << std::endl;
    }
// need this for CGI no? so maybe before?
    if (_root.empty())
        _root = resolveRoot(_serverConfig, HTTPParser.getRequestTarget());

    // When a range branch builds its own 206/416 header for the partial guys heehee skip buildAnswerHeader.
    bool rangeHandled = false;

    if (HTTPParser.getMethod() == "DELETE"){
        
        if (HTTPParser.getError() == true){
            std::string file = getFile(HTTPParser.getCode(), HTTPParser.getError()); 
            answerFile(file);
        }
        else{
            
            std::string file = getFileUpload(HTTPParser.getCode()); 
            
            if (removeFile(file) == false)
                sendError(HTTPParser);
            else{
                
                HTTPParser.setCode("204");
                HTTPParser.setType("text/html");
            }
        }
    }
    else if (HTTPParser.getMethod() == "POST"){
        
        if (uploadFile(HTTPParser.getFileName(), HTTPParser.getFileBuf()) == false)
            sendError(HTTPParser);
        else{
            
            HTTPParser.setCode("201");
            HTTPParser.getType();
        }
    }
    else if (HTTPParser.getCode() == "autoindex")
    {
        // HTTPParser.getPath() is the resolved on-disk directory (root + URI).
        _body = generateAutoindex(HTTPParser.getPath(), HTTPParser.getRequestTarget());
        if (_body.empty())
       {
            HTTPParser.setError(true);
            HTTPParser.setCode("403"); // if real dir didn't open
            HTTPParser.setType("text/html");
        }
        else
        {
            // put the listing where epoll reads the response body (getBuffer()/getNReadIndex()) and let Content-Length mirror
            if (_body.size() > (size_t)BUF_SIZE)
                _body.resize(BUF_SIZE);
            memcpy(_buffer, _body.data(), _body.size());
            _n_read_index = (ssize_t)_body.size();
        }
    }
    else if (HTTPParser.getType() == "text/html"){
        
        std::string file = getFile(HTTPParser.getCode(), HTTPParser.getError()); 
        if (answerFile(file) == false)
            sendError(HTTPParser);
    }
    else if (HTTPParser.getType() == "text/plain"){
        
        std::string file = getFileUpload(HTTPParser.getCode()); 
        if (answerFile(file) == false)
            sendError(HTTPParser);
    }
    else if (HTTPParser.getType() == "image/jpeg" || HTTPParser.getType() == "image/png" || HTTPParser.getType() == "image/gif" || HTTPParser.getType() == "image/webp"){

        std::string file = getFileImage(HTTPParser.getCode());

        //  206 Partial Content path (only range set) 
        struct stat sb;
        std::string range = HTTPParser.getRange();
        if (!range.empty() && stat(file.c_str(), &sb) == 0)
        {
            ByteRange r = parseRangeHeader(range, (long)sb.st_size);
            if (r.unsatisfiable)
            {
                build416Header((long)sb.st_size);
                rangeHandled = true;
            }
            else if (r.valid && answerFilePartial(file, r))
            {
                buildPartialHeader(HTTPParser.getType(), r, (long)sb.st_size);
                rangeHandled = true;
            }
        }
        // OR just normal 200 full-file response
        if (!rangeHandled && answerFile(file) == false){
            HTTPParser.setError(true);
            HTTPParser.setCode("404");
            HTTPParser.setType("text/html");
        
        if (HTTPParser.getUpload() == true){
            std::string file = getFileUpload(HTTPParser.getCode()); 
            if (answerFile(file) == false)
                sendError(HTTPParser);
        }
        else{
            std::string file = getFileImage(HTTPParser.getCode()); 
            if (answerFile(file) == false)
                sendError(HTTPParser);
        }
    }
    else if (HTTPParser.getType() == "image/x-icon"){
        
        if (answerFileIcon() == false)
            sendError(HTTPParser);
    }
    else if (HTTPParser.getType() == "multipart/form-data"){
        
        if (uploadFile(HTTPParser.getFileName(), HTTPParser.getFileBuf()) == false)
            sendError(HTTPParser);
        else{
            
            HTTPParser.setCode("201");
            char const *lastPoint = strrchr(HTTPParser.getFileName().c_str(), '.');
            std::string suffix = std::string(lastPoint, strlen(lastPoint));
            HTTPParser.setType(HTTPParser.addSuffix(suffix));
        }
    }

    if (!rangeHandled) // a 206/416 header was already built by the range path
        buildAnswerHeader(HTTPParser.getCode(), HTTPParser.getType());

    return true;

}

// SO GIRLS: flow is set at the top of handleRequest()'s file branches:
//  HTTPParser.getRange() gives the raw "Range:" value ("", if none)
//  parseRangeHeader(value, fileSize) -> ByteRange
//  r.unsatisfiable -> send build416Header(); r.valid -> answerFilePartial 
//+ buildPartialHeader (206); otherwise fall through to the REGULar 200 // 
//  single range only; multipart/multi-range (chec k with other teams but it would be such a pain uguys)
// Parse a single "bytes=..." range against a known file size.
// supported forms are the follw: "bytes=start-end", "bytes=start-" (to EOF), "bytes=-suffix".
RequestHandler::ByteRange
// Range : start - end
// range : start - EOF
// range : -N bytes

ByteRange RequestHandler::parseRangeHeader(std::string const& rangeValue, long fileSize)
{
    ByteRange r;
    r.start = 0;
    r.end = fileSize > 0 ? fileSize - 1 : 0;
    r.valid = false;
    r.unsatisfiable = false;

    // only the bytes unit is ok; anything else considered no range (200).
    const std::string prefix = "bytes=";
    if (rangeValue.compare(0, prefix.size(), prefix) != 0)
        return r;
    std::string spec = rangeValue.substr(prefix.size());

    size_t dash = spec.find('-');
    if (dash == std::string::npos)
        return r; // malformed ; ignore, serve 200

    std::string startStr = spec.substr(0, dash);
    std::string endStr = spec.substr(dash + 1);

    long start;
    long end;
    if (startStr.empty())
    {
        // suffix form "-N" - last N bytes
        if (endStr.empty())
            return r;
        long suffix = atol(endStr.c_str());
        if (suffix <= 0)
        {
            r.unsatisfiable = true; // 416
            return r;
        }
        if (suffix > fileSize)
            suffix = fileSize;
        start = fileSize - suffix;
        end = fileSize - 1;
    }
    else
    {
        start = atol(startStr.c_str());
        end = endStr.empty() ? fileSize - 1 : atol(endStr.c_str());
        if (end > fileSize - 1)
            end = fileSize - 1; // it's the clamp of EOF
    }

    if (start < 0 || start >= fileSize || start > end)
    {
        r.unsatisfiable = true; // 416;if start > end or start >= fileSize → the range is unsatisfiable: 
        return r;
    }

    r.start = start;
    r.end = end;
    r.valid = true;
    return r;
}

// read bytes (r.start to r.end) of FILE into _buffer and set _n_read_index to that length, so RS sends only the slice
bool RequestHandler::answerFilePartial(std::string const & file, ByteRange const & r)
{
    long length = r.end - r.start + 1;
    if (length <= 0 || length > BUF_SIZE)
    {
        std::cerr << "Partial length out of range: " << length << std::endl;
        return false;
    }

    int fd = open(file.c_str(), O_RDONLY);
    if (fd == -1)
    {
        std::cerr << "Error opening file for partial read: " << strerror(errno) << std::endl;
        return false;
    }
    if (lseek(fd, r.start, SEEK_SET) == (off_t)-1)
    {
        std::cerr << "lseek failed: " << strerror(errno) << std::endl;
        close(fd);
        return false;
    }
    _n_read_index = read(fd, _buffer, length);
    close(fd);
    if (_n_read_index != length)
    {
        std::cerr << "Short partial read: " << _n_read_index << "/" << length << std::endl;
        return false;
    }
    return true;
}

// build the 206 header HEREEE; Content-Length is the SLICED length; content range's final number is the TOTAL file size
std::string RequestHandler::buildPartialHeader(std::string const & type, ByteRange const & r, long fileSize)
{
    std::stringstream ss;
    ss << "HTTP/1.1 206 Partial Content\r\n"
       << "Content-Type: " << type << "\r\n"
       << "Accept-Ranges: bytes\r\n"
       << "Content-Range: bytes " << r.start << "-" << r.end << "/" << fileSize << "\r\n"
       << "Content-Length: " << (r.end - r.start + 1) << "\r\n\r\n";
    _header = ss.str();
    return _header;
}

// 416 : set body empty and content-range shows size
std::string RequestHandler::build416Header(long fileSize)
{
    std::stringstream ss;
    ss << "HTTP/1.1 416 Range Not Satisfiable\r\n"
       << "Content-Range: bytes */" << fileSize << "\r\n"
       << "Content-Length: 0\r\n\r\n";
    _header = ss.str();
    _n_read_index = 0; // setting empty bod here
    return _header;
}
