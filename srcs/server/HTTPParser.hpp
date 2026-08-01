/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ankim <ankim@student.42.fr>                +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 15:54:09 by lzannis           #+#    #+#             */
/*   Updated: 2026/08/01 17:57:14 by ankim            ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


# pragma once

#include "../lexer/Lexer.hpp"
#include "ListenerManager.hpp"
#include "../parser/config/ServerConfig.hpp"

enum HttpCode
{
    HTTP_400,
    HTTP_403,
    HTTP_404,
    HTTP_405,
    HTTP_413,
    HTTP_421,
    HTTP_201,
    HTTP_204,
    HTTP_500,
    HTTP_CGI,
    HTTP_INDEX,
    HTTP_AUTOINDEX,
    HTTP_FAVICON,
    HTTP_FILE
};

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <stdexcept>
#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <cstring>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <limits>


#define BUF_SIZE 800000

typedef enum RequestParser
{
    REQUESTLINE,
    HOST,
    USER_AGENT,
    ACCEPT,
    ACCEPT_LANGUAGE,
    ACCEPT_ENCODING,
    CONTENT_TYPE,
    CONTENT_LENGTH,
    CONNECTION,
    REFERER,
    UPGRADE_INSECURE_REQUESTS,
    SEC_FETCH_DEST,
    SEC_FETCH_MODE,
    SEC_FETCH_SITE,
    SEC_FETCH_USER,
    PRIORITY,
    // CHROME
    SEC_CH_UA_PLATFORM,
    SEC_CH_UA,
    SEC_CH_UA_MOBILE,
    ORIGIN, // FOR CGI FOR CHROME
    //BRAVE
    CACHE_CONTROL,
    SEC_GPC,
    UNKNOWN_BROWSER,
} RequestParser ;

class HTTPParser
{

private:

    std::string                 _request;
    const ServerConfig          &_serverConfig;
    HttpCode                    _code;
    std::string                 _type;
    std::string                 _method;
    std::string                 _requesttarget;
    std::string                 _httpversion;
    std::string                 _boundary;
    std::string                 _fileLength;
    std::string                 _fileName;
    std::string                 _fileBuf;
    std::string::iterator       _pos;
    bool                        _errors;
    bool                        _upload;

    
/* ADD INS FOR CGI------*/
    bool                        _isCGI;
    std::string                 _fullPath;
    std::string                 _query_string;
    std::string                 _scriptFilename;
    std::string                 _body;
    std::string                 _content_type;
    std::string                 _content_length;
    int                         _content_int;

    bool                _autoindexOn; 
    std::string         _rangeHeader;

public:

                                HTTPParser( std::string const & request, const ServerConfig &serverConfig  );
                                HTTPParser( HTTPParser const & src );    
                                ~HTTPParser();    
    HTTPParser &                operator=( HTTPParser const & other );
            
    HttpCode                    getCode() const;
    std::string                 getType() const;
    std::string                 getMethod() const;
    std::string                 getBoundary() const;
    std::string                 getFileName() const;
    std::string                 getFileBuf() const;
    bool                        getError() const;
    bool                        getUpload() const;


    HttpCode                    setCode( HttpCode code );
    std::string                 setType( std::string const & type );
    bool                        setError( bool error );    


    /* ADD INS FOR CGI-------------*/
    std::string                 getPath() const;
    std::string                 getScriptFilename() const;
    std::string                 getQueryString() const;
    std::string                 getBody() const;
    std::string                 getContentType() const;
    std::string                 getContentLength() const;
    std::string                 getRequestTarget() const;
    bool                        isCGI() const;

    const LocationConfig*       matchLocation() const;
    void                        buildFullPath();

    void                        extractRange();
    std::string                 getRange() const;

    void                        parseCGI();
    void                        extractBody();
    bool                        validateCGIRequest();
    /*------------------------- */

    bool                        varNotFound400 ( size_t var );
    bool                        doesCharCExist400 ( char const *str );
    
    std::vector<size_t>         collectSpace( std::string::iterator start, std::string::iterator end );
    std::vector<std::string>    collectString( std::string & line, std::vector<size_t> & space_inter );

    bool                        checkSize();
    bool                        checkRequestLine();
    bool                        checkHost( std::string const & value, ListenerManager const & listener );
    bool                        isRequestValid( ListenerManager const & listen );
                
    bool                        checkContentType();
    bool                        checkContentLength();
    bool                        checkContentDisposition();
    bool                        gatherFile();
            
    std::string                 addSuffix(std::string suffix);
    bool                        compareMethodWithConfigFile();



    bool                        findMethods();
    bool                        findPath();
    bool                        findHeaders();

    static std::string          httpCodeToString( HttpCode code );
};
