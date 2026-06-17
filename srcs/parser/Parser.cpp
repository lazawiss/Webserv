#include "Parser.hpp"

#include "../lexer/Lexer.hpp"

/*
** ============================================================================
** Parser - Orthodox canonical form
** ============================================================================
*/

Parser::Parser(const std::vector<Token> &tokens) : _tokens(tokens), _index(0) {}

Parser::~Parser() {}

/*
** ============================================================================
** Parser - Token navigation methods
** ============================================================================
*/

/*
** Returns the current token without moving forward in the token chain.
** Used to inspect what we are about to parse.
*/
const Token& Parser::current() const
{
    return _tokens[_index];
}

/*
** Returns the current token and moves forward by one in the token chain.
** Used to next a token once we know it is valid.
*/
const Token& Parser::next()
{
    return _tokens[_index++];
}

/*
** ============================================================================
** Parser - Parse file
** ============================================================================
*/

/**
** @brief Opens, validates, parses and closes a webserv configuration file.
**
** Reads the file line by line, tokenizes each line using the Lexer,
** and merges all tokens into a single chain terminated by an End token.
** The chain is then handed to the Parser, which performs a recursive
** descent parse and builds the resulting GlobalConfig.
**
** @param str Path to the configuration file to parse.
** 
** @return ...
**/
int parse_file(const std::string &str)
{
    std::ifstream file(str.c_str());

    if (file.is_open() == false)
    {
        std::cerr << "Error: file '" << str << "' doesn't exist" << std::endl;
        return ERROR;
    }

    if (file.peek() == std::ifstream::traits_type::eof())
    {
        std::cerr << "Error: file '" << str << "' is empty" << std::endl;
        return ERROR;
    }

    std::string line;
    std::vector<Token> allTokens;

    while (std::getline(file, line))
    {   
        Lexer lexer(line);
        std::vector<Token> lineTokens = lexer.tokenize();

        for (size_t i = 0; i < lineTokens.size(); i++)
        {
            if (lineTokens[i].type != End)
                allTokens.push_back(lineTokens[i]);
        }
    }

    allTokens.push_back(Token(End, ""));

    try
    {
        Parser parser(allTokens);
        GlobalConfig config = parser.parse();

    /* ============================================================================ */
    /* ============================================================================ */
    /* ============================================================================ */

    std::cout << "===========================" << std::endl;
    std::cout << "[GLOBAL]" << std::endl;
    std::cout << std::endl;
    std::cout << "root:                 " << config.getRoot()            << std::endl;
    std::cout << "autoindex:            " << config.getAutoindex()       << std::endl;
    std::cout << "client_max_body_size: " << config.getClientMaxBodySize() << std::endl;

    std::cout << "index:                ";
    for (size_t i = 0; i < config.getIndex().size(); i++)
        std::cout << config.getIndex()[i] << " ";
    std::cout << std::endl;

    std::cout << "error_pages:          ";
    const std::map<int, std::string>& ep = config.getErrorPages();
    for (std::map<int, std::string>::const_iterator it = ep.begin(); it != ep.end(); it++)
        std::cout << it->first << " -> " << it->second << " ";
    std::cout << std::endl;
    std::cout << std::endl;

    std::cout << "===========================" << std::endl;
    std::cout << "[SERVER] [LOCATION]" << std::endl;
    std::cout << std::endl;
    std::cout << "nb servers:           " << config.getServers().size() << std::endl;

    for (size_t i = 0; i < config.getServers().size(); i++)
    {
        std::cout << std::endl;
        std::cout << "-------- server[" << i << "] --------" << std::endl;

        std::cout << "listen:               ";
        for (size_t j = 0; j < config.getServers()[i].getListen().size(); j++)
            std::cout << config.getServers()[i].getListen()[j] << " ";
        std::cout << std::endl;

        std::cout << "server_name:          ";
        for (size_t j = 0; j < config.getServers()[i].getServerNames().size(); j++)
            std::cout << config.getServers()[i].getServerNames()[j] << " ";
        std::cout << std::endl;

        std::cout << "root:                 " << config.getServers()[i].getRoot()            << std::endl;
        std::cout << "autoindex:            " << config.getServers()[i].getAutoindex()       << std::endl;
        std::cout << "client_max_body_size: " << config.getServers()[i].getClientMaxBodySize() << std::endl;

        std::cout << "index:                ";
        for (size_t j = 0; j < config.getServers()[i].getIndex().size(); j++)
            std::cout << config.getServers()[i].getIndex()[j] << " ";
        std::cout << std::endl;

        std::cout << "error_pages:          ";
        const std::map<int, std::string>& sep = config.getServers()[i].getErrorPages();
        for (std::map<int, std::string>::const_iterator it = sep.begin(); it != sep.end(); it++)
            std::cout << it->first << " -> " << it->second << " ";
        std::cout << std::endl;

        std::cout << "nb locations:         " << config.getServers()[i].getLocations().size() << std::endl;
        std::cout << std::endl;

        for (size_t j = 0; j < config.getServers()[i].getLocations().size(); j++)
        {
            std::cout << "  ----- location[" << j << "] -----" << std::endl;
            std::cout << "  path:               " << config.getServers()[i].getLocations()[j].getPath()      << std::endl;
            std::cout << "  root:               " << config.getServers()[i].getLocations()[j].getRoot()      << std::endl;
            std::cout << "  autoindex:          " << config.getServers()[i].getLocations()[j].getAutoindex() << std::endl;

            std::cout << "  index:              ";
            for (size_t k = 0; k < config.getServers()[i].getLocations()[j].getIndex().size(); k++)
                std::cout << config.getServers()[i].getLocations()[j].getIndex()[k] << " ";
            std::cout << std::endl;

            std::cout << "  methods:            ";
            for (size_t k = 0; k < config.getServers()[i].getLocations()[j].getMethods().size(); k++)
                std::cout << config.getServers()[i].getLocations()[j].getMethods()[k] << " ";
            std::cout << std::endl;

            std::cout << "  error_pages:        ";
            const std::map<int, std::string>& lep = config.getServers()[i].getLocations()[j].getErrorPages();
            for (std::map<int, std::string>::const_iterator it = lep.begin(); it != lep.end(); it++)
                std::cout << it->first << " -> " << it->second << " ";
            std::cout << std::endl;
        }
    }
    std::cout << std::endl;
    std::cout << "==========================" << std::endl;
    
    /* ============================================================================ */
    /* ============================================================================ */
    /* ============================================================================ */

    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }

    return SUCCESS;
}

/*
** ============================================================================
** Parser – Recursive descent parser
** ============================================================================
*/

/**
** @brief Entry point of the recursive descent parser.
**
** Builds the Abstract Syntax Tree (AST) of the configuration file by
** reading the token chain and producing a GlobalConfig object
** containing all server blocks.
**
** @throws ExpectedWord if an unexpected token is found in the global context.
** @throws NoServerDefined if no server was found.
** 
** @return GlobalConfig object.
**/
GlobalConfig Parser::parse()
{
    GlobalConfig config;

    while (current().type != End)
    {
        if (current().type == Word && current().value == "server")
            config.addServer(parseServer());
        else if (current().type == Word)
            parseInheritableDirective(config);
        else
            throw ExpectedWord();
    }

    if (config.getServers().empty())
        throw NoServerDefined();

    return config;
}

/**
** @brief Recursive descent step that parses a single "server { ... }"
** block, one node of the configuration AST.
**
** Consumes the "server" keyword, the opening and closing brackets,
** and dispatches each directive found inside to the matching parser.
**
** @throws ExpectedLBracket if "{" is missing after "server".
** @throws ExpectedRBracket if "}" is missing at the end of the block.
** @throws ExpectedWord if an unexpected token is found inside the block.
**
** @return ServerConfig object.
**/
ServerConfig Parser::parseServer()
{
    ServerConfig server;

    next();
    if (current().type != LBracket)
        throw ExpectedLBracket();
    next();

    while (current().type != RBracket && current().type != End)
    {
        if (current().type == Word && current().value == "listen")
            parseDirectiveListen(server);
        else if (current().type == Word && current().value == "server_name")
            parseDirectiveServerName(server);
        else if (current().type == Word && current().value == "location")
            server.addLocation(parseLocation());
        else if (current().type == Word)
            parseInheritableDirective(server);
        else
            throw ExpectedWord();
    }
    
    if (current().type != RBracket)
        throw ExpectedRBracket();

    next();

    return server;
}

/**
** @brief Recursive descent step that parses a single
** "location <path> { ... }" block, the deepest node of the configuration AST.
**
** Consumes the "location" keyword, the path, the opening and closing
** brackets, and dispatches each directive found inside to the matching parser.
**
** @throws ExpectedWord if the path is missing or invalid.
** @throws ExpectedLBracket if "{" is missing after the path.
** @throws ExpectedRBracket if "}" is missing at the end of the block.
**
** @return LocationConfig object.
*/
LocationConfig Parser::parseLocation()
{
    LocationConfig location;

    next();
    
    if (current().type != Word || current().value[0] != '/')
        throw ExpectedWord();

    location.setPath(next().value);

    if (current().type != LBracket)
        throw ExpectedLBracket();
    next();

    while (current().type != RBracket && current().type != End)
    {
        if (current().type == Word && current().value == "methods")
            parseDirectiveMethods(location);
        else if (current().type == Word)
            parseInheritableDirective(location);
        else
            throw ExpectedWord();
    }
    
    if (current().type != RBracket)
        throw ExpectedRBracket();

    next();

    return location;
}

/**
** @brief Dispatches a directive shared by the global, server and
** location nodes of the AST (root, index, error_page, autoindex,
** client_max_body_size) to its matching parser, using the common
** AConfig interface.
**
** @param ref The AConfig context to fill (GlobalConfig, ServerConfig
**            or LocationConfig).
** @throws UnknownDirective if the directive is not recognized.
*/
void Parser::parseInheritableDirective(AConfig &ref)
{
    if (current().value == "root")
        parseDirectiveRoot(ref);
    else if (current().value == "index")
        parseDirectiveIndex(ref);
    else if (current().value == "error_page")
        parseDirectiveErrorPage(ref);
    else if (current().value == "autoindex")
        parseDirectiveAutoIndex(ref);
    else if (current().value == "client_max_body_size")
        parseDirectiveClientMaxBodySize(ref);
    else
        throw UnknownDirective();
}

/*
** ============================================================================
** Parser – Member methods
** ============================================================================
*/

void Parser::parseDirectiveRoot(AConfig &ref)
{
    next();
    if (current().type != Word)
        throw ExpectedWord();
    ref.setRoot(next().value);
    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

void Parser::parseDirectiveIndex(AConfig &ref)
{
    next();
    if (current().type != Word)
        throw ExpectedWord();
    while (current().type == Word)
    {
        ref.addIndex(next().value);
    }
    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

void Parser::parseDirectiveAutoIndex(AConfig &ref)
{
    next();
    if (current().type != Word)
        throw ExpectedWord();
    if (current().value != "on" && current().value != "off")
        throw ExpectedCorrectAutoIndex();
    ref.setAutoindex(next().value == "on" ? true : false);
    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

void Parser::parseDirectiveClientMaxBodySize(AConfig &ref)
{
    (void)ref;
    next();
    if (current().type != Word)
        throw ExpectedWord();
    ref.setClientMaxBodySize(parseSize(next().value));
    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

size_t Parser::parseSize(const std::string &word) const
{
    size_t i = 0;

    while (i < word.size() && std::isdigit(word[i]))
        i++;
    
    if (i == 0)
        throw ExpectedCorrectSize();

    size_t value = std::atoi(word.c_str());
    
    if (i == word.size())
        return value;
    
    if (i != word.size() - 1)
        throw ExpectedCorrectSize();

    char unit = word[i];
    if (unit == 'K' || unit == 'k')
        return value * 1024;
    if (unit == 'M' || unit == 'm')
        return value * 1024 * 1024;
    if (unit == 'G' || unit == 'g')
        return value * 1024 * 1024 * 1024;

    throw ExpectedCorrectUnit();
}

void Parser::parseDirectiveErrorPage(AConfig &ref)
{
    next();
    if (current().type != Word)
        throw ExpectedWord();

    int code = parseCode(next().value);

    if (code < 400 || code > 599)
        throw ExpectedCorrectCode();

    if (current().type != Word)
        throw ExpectedWord();

    std::string uri = next().value;
    ref.addErrorPage(code, uri);

    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

size_t Parser::parseCode(const std::string &word) const
{
    if (word.size() != 3)
        throw ExpectedCorrectCode();

    for (size_t i = 0; i < word.size(); i++)
    {
        if (!std::isdigit(word[i]))
            throw ExpectedCorrectCode();
    }

    return std::atoi(word.c_str());
}

void Parser::parseDirectiveListen(ServerConfig &ref)
{
    next();
    if (current().type != Word)
        throw ExpectedWord();
    ref.addListen(next().value);
    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

void Parser::parseDirectiveServerName(ServerConfig &ref)
{
    next();
    if (current().type != Word)
        throw ExpectedWord();
    while (current().type == Word)
    {
        ref.addServerName(next().value);
    }
    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

void Parser::parseDirectiveMethods(LocationConfig &ref)
{
    next();
    if (current().type != Word)
        throw ExpectedWord();
    while (current().type == Word)
    {
        if (current().value != "GET" && current().value != "POST"
            && current().value != "PUT" && current().value != "DELETE")
                throw ExpectedCorrectMethod();

        ref.addMethod(next().value);
    }
    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

/*
** ============================================================================
** Parser – handle error with try/catch
** ============================================================================
*/

const char* Parser::NoServerDefined::what() const throw()
{
    return "At least one server block is required";
}

const char* Parser::ExpectedWord::what() const throw()
{
    return "Unexpected token, should be a 'word' type";
}

const char* Parser::ExpectedSemicolon::what() const throw()
{
    return "Unexpected token, should be a 'semi colon' type";
}

const char* Parser::ExpectedLBracket::what() const throw()
{
    return "Unexpected token, should be a 'left braket' type";
}

const char* Parser::ExpectedRBracket::what() const throw()
{
    return "Unexpected token, should be a 'right braket' type";
}

const char* Parser::UnknownDirective::what() const throw()
{
    return "Unexpected token, unknown directive in global, "
        "server or location context";
}

const char* Parser::ExpectedCorrectMethod::what() const throw()
{
    return "Invalid HTTP method, should be 'GET', 'POST', 'PUT' or 'DELETE'";
}

const char* Parser::ExpectedCorrectAutoIndex::what() const throw()
{
    return "Unexpected token, auto index can be 'on' or 'off'";
}

const char* Parser::ExpectedCorrectSize::what() const throw()
{
    return "Invalid size value: expected at least one digit (e.g. '10M', "
        "'512K', '1G', or '1024')";
}

const char* Parser::ExpectedCorrectUnit::what() const throw()
{
    return "Invalid size unit: expected 'K', 'M' or 'G' after the number "
        "(e.g. '10M', '512K', '1G')";
}

const char* Parser::ExpectedCorrectCode::what() const throw()
{
    return "Invalid HTTP error code: expected a value between 400 and "
        "599 (e.g. '404', '500')";
}