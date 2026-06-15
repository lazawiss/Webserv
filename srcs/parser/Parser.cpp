#include "Parser.hpp"

#include "../lexer/Lexer.hpp"

// void print_token_chain(const std::vector <Token> &tokens);

/*
** ============================================================================
** Parser - Constructors & Destructor
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

/*
** Opens and validates the config file, then tokenizes its content
** line by line into a single token chain.
** Returns SUCCESS if the file was parsed correctly, ERROR otherwise.
*/
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
    // print_token_chain(allTokens);

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
    // std::cout << "client_max_body_size: " << config.getClientMaxBodySize() << std::endl;

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
        // std::cout << "client_max_body_size: " << config.getServers()[i].getClientMaxBodySize() << std::endl;

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
** Parser – Member functions
** ============================================================================
*/

/*
** Entry point of the parser. Reads the token chain and builds
** a GlobalConfig object containing all server blocks.
** Throws ParseError if the syntax is invalid.
*/
GlobalConfig Parser::parse()
{
    GlobalConfig config;

    while (current().type != End)
    {
        if (current().type == Word && current().value == "server")
            config.addServer(parseServer());
        else if (current().type == Word)
            parseConfig(config);
        else
            throw ExpectedWord();
    }

    if (config.getServers().empty())
        throw NoServerDefined();

    return config;
}

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
            parseConfigListen(server);
        else if (current().type == Word && current().value == "server_name")
            parseConfigServerName(server);
        else if (current().type == Word && current().value == "location")
            server.addLocation(parseLocation());
        else if (current().type == Word)
            parseServerDirective(server);
        else
            throw ExpectedWord();
    }
    
    if (current().type != RBracket)
        throw ExpectedRBracket();

    next();

    return server;
}

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
            parseConfigMethod(location);
        else if (current().type == Word)
            parseLocationDirective(location);
        else
            throw ExpectedWord();
    }
    
    if (current().type != RBracket)
        throw ExpectedRBracket();

    next();

    return location;
}

void Parser::parseConfig(GlobalConfig &config)
{
    if (current().value == "root")
        parseConfigRoot(config);
    else if (current().value == "index")
        parseConfigIndex(config);
    else if (current().value == "error_page")
        parseConfigErrorPage(config);
    else if (current().value == "autoindex")
        parseConfigAutoIndex(config);
    else if (current().value == "client_max_body_size")
        parseConfigClientMaxBodySize(config);
    else
        throw UnknownDirective();
}

void Parser::parseServerDirective(ServerConfig& server)
{
    if (current().value == "root")
        parseConfigRoot(server);
    else if (current().value == "index")
        parseConfigIndex(server);
    else if (current().value == "error_page")
        parseConfigErrorPage(server);
    else if (current().value == "autoindex")
        parseConfigAutoIndex(server);
    else if (current().value == "client_max_body_size")
        parseConfigClientMaxBodySize(server);
    else
        throw UnknownDirective();
}

void Parser::parseLocationDirective(LocationConfig &location)
{
    if (current().value == "root")
        parseConfigRoot(location);
    else if (current().value == "index")
        parseConfigIndex(location);
    else if (current().value == "error_page")
        parseConfigErrorPage(location);
    else if (current().value == "autoindex")
        parseConfigAutoIndex(location);
    else if (current().value == "client_max_body_size")
        parseConfigClientMaxBodySize(location);
    else
        throw UnknownDirective();
}

void Parser::parseConfigRoot(AConfig &ref)
{
    next();
    if (current().type != Word)
        throw ExpectedWord();
    ref.setRoot(next().value);
    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

void Parser::parseConfigIndex(AConfig &ref)
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

void Parser::parseConfigAutoIndex(AConfig &ref)
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

void Parser::parseConfigClientMaxBodySize(AConfig &ref)
{
    (void)ref;

    next();
    if (current().type != Word)
        throw ExpectedWord();
    // TO DO (later): create a parseSize function/method
    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

void Parser::parseConfigErrorPage(AConfig &ref)
{
    next();
    if (current().type != Word)
        throw ExpectedWord();
    
    int code = std::atoi(next().value.c_str());

    if (current().type != Word)
        throw ExpectedWord();

    std::string uri = next().value;
    ref.addErrorPage(code, uri);

    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

void Parser::parseConfigListen(ServerConfig &ref)
{
    next();
    if (current().type != Word)
        throw ExpectedWord();
    ref.addListen(next().value);
    if (current().type != Semicolon)
        throw ExpectedSemicolon();
    next();
}

void Parser::parseConfigServerName(ServerConfig &ref)
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

void Parser::parseConfigMethod(LocationConfig &ref)
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
    return "Error: unexpected token, unknown directive in global, "
        "server or location context";
}

const char* Parser::ExpectedCorrectMethod::what() const throw()
{
    return "Invalid HTTP method, should be 'GET', 'POST', 'PUT' or 'DELETE'";
}

const char* Parser::ExpectedCorrectAutoIndex::what() const throw()
{
    return "Error: unexpected token, auto index can be 'on' or 'off'";
}

/*
** ============================================================================
** Print Tokens - DEBUG!!
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

// void print_token_chain(const std::vector <Token> &tokens)
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