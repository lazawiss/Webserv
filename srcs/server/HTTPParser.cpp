/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   HTTPParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/04 17:20:07 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/20 17:04:58 by lzannis          ###   ########.fr       */
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

    // if (file.peek() == std::ifstream::traits_type::eof())
    // {
    //     std::cerr << "Error: file '" << str << "' is empty" << std::endl;
    //     throw std::logic_error("File is empty");
    // }


    std::istringstream iss(str);
    std::string line;
    // std::vector<Token> allTokens;

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
    print_token_chain(_allTokens);
    

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


