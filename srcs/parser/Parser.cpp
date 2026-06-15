#include "Parser.hpp"

#include "../lexer/Lexer.hpp"

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
** Returns the next token without moving forward in the token chain.
** Used to look ahead and validate what comes after the current token.
*/
const Token& Parser::peek() const
{
    return _tokens[_index + 1];
}

/*
** Returns the current token and moves forward by one in the token chain.
** Used to consume a token once we know it is valid.
*/
const Token& Parser::consume()
{
    return _tokens[_index++];
}

/*
** ============================================================================
** 
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

    Parser parser(allTokens);
    GlobalConfig config = parser.parse();



    /* ============================================================================ */

    std::cout << "==========================" << std::endl;
    std::cout << "[GLOBAL]" << std::endl;
    std::cout << "root:                 " << config.getRoot()            << std::endl;
    std::cout << "autoindex:            " << config.getAutoindex()       << std::endl;
    // std::cout << "client_max_body_size: " << config.getClientMaxBodySize() << std::endl;

    std::cout << "index: ";
    for (size_t i = 0; i < config.getIndex().size(); i++)
        std::cout << config.getIndex()[i] << " ";
    std::cout << std::endl;

    std::cout << "error_pages: ";
    const std::map<int, std::string>& ep = config.getErrorPages();
    for (std::map<int, std::string>::const_iterator it = ep.begin(); it != ep.end(); it++)
        std::cout << it->first << " -> " << it->second << " ";
    std::cout << std::endl;

    std::cout << "==========================" << std::endl;
    std::cout << "nb servers: " << config.getServers().size() << std::endl;

    for (size_t i = 0; i < config.getServers().size(); i++)
    {
        std::cout << "--- server[" << i << "] ---" << std::endl;

        std::cout << "listen: ";
        for (size_t j = 0; j < config.getServers()[i].getListen().size(); j++)
            std::cout << config.getServers()[i].getListen()[j] << " ";
        std::cout << std::endl;

        std::cout << "server_name: ";
        for (size_t j = 0; j < config.getServers()[i].getServerNames().size(); j++)
            std::cout << config.getServers()[i].getServerNames()[j] << " ";
        std::cout << std::endl;

        std::cout << "root:                 " << config.getServers()[i].getRoot()            << std::endl;
        std::cout << "autoindex:            " << config.getServers()[i].getAutoindex()       << std::endl;
        // std::cout << "client_max_body_size: " << config.getServers()[i].getClientMaxBodySize() << std::endl;

        std::cout << "index: ";
        for (size_t j = 0; j < config.getServers()[i].getIndex().size(); j++)
            std::cout << config.getServers()[i].getIndex()[j] << " ";
        std::cout << std::endl;

        std::cout << "error_pages: ";
        const std::map<int, std::string>& sep = config.getServers()[i].getErrorPages();
        for (std::map<int, std::string>::const_iterator it = sep.begin(); it != sep.end(); it++)
            std::cout << it->first << " -> " << it->second << " ";
        std::cout << std::endl;
    }
    std::cout << "==========================" << std::endl;
    /* ============================================================================ */

    return SUCCESS;
}

/*
** ============================================================================
** 
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
        { /* TO DO: Throw an error */}
    }

    if (config.getServers().empty())
    { /* TO DO: Throw an error : At least one server block is required" */ }

    return config;
}

void Parser::parseConfig(GlobalConfig &config)
{
    if (current().value == "root")                      parseConfigRoot(config);
    else if (current().value == "index")                parseConfigIndex(config);
    else if (current().value == "error_page")           parseConfigErrorPage(config);
    else if (current().value == "autoindex")            parseConfigAutoIndex(config);
    else if (current().value == "client_max_body_size") parseConfigClientMaxBodySize(config);
    else                                                { /* TO DO: Throw an error */}
}

ServerConfig Parser::parseServer()
{
    ServerConfig server;

    consume();

    if (current().type != LBracket)
    { /* TO DO: Throw an error */}
    consume();

    while (current().type != RBracket && current().type != End)
    {
        if (current().type == Word && current().value == "listen")              parseConfigListen(server);
        else if (current().type == Word && current().value == "server_name")    parseConfigServerName(server);
        else if (current().type == Word && current().value == "location")       server.addLocation(parseLocation());
        else if (current().type == Word)                                        parseServerDirective(server);
        else { /* TO DO: Throw an error */}
    }
    
    if (current().type != RBracket)
    { /* TO DO: Throw an error */}

    consume();

    return server;
}

LocationConfig Parser::parseLocation()
{
    LocationConfig location;

    consume();
    
    if (current().type != Word || current().value[0] != '/')
    { /* TO DO: Throw an error */}

    location.setPath(consume().value);

    if (current().type != LBracket)
    { /* TO DO: Throw an error */}
    consume();

    while (current().type != RBracket && current().type != End)
    {
        if (current().type == Word && current().value == "method")              parseConfigMethod(location);
        else if (current().type == Word)                                        parseLocationDirective(location);
        else { /* TO DO: Throw an error */}
    }
    
    if (current().type != RBracket)
    { /* TO DO: Throw an error */}

    consume();

    return location;
}

void Parser::parseServerDirective(ServerConfig& server)
{
    if (current().value == "root")                      parseConfigRoot(server);
    else if (current().value == "index")                parseConfigIndex(server);
    else if (current().value == "error_page")           parseConfigErrorPage(server);
    else if (current().value == "autoindex")            parseConfigAutoIndex(server);
    else if (current().value == "client_max_body_size") parseConfigClientMaxBodySize(server);
    else                                                { /* TO DO: Throw an error */}
}

void Parser::parseLocationDirective(LocationConfig &location)
{
    if (current().value == "root")                      parseConfigRoot(location);
    else if (current().value == "index")                parseConfigIndex(location);
    else if (current().value == "error_page")           parseConfigErrorPage(location);
    else if (current().value == "autoindex")            parseConfigAutoIndex(location);
    else if (current().value == "client_max_body_size") parseConfigClientMaxBodySize(location);
    else                                                { /* TO DO: Throw an error */}
}

void Parser::parseConfigRoot(AConfig &ref)
{
    consume();
    if (current().type != Word)
    { /* TO DO: Throw an error */}
    ref.setRoot(consume().value);
    if (current().type != Semicolon)
    { /* TO DO: Throw an error */}
    consume();
}

void Parser::parseConfigIndex(AConfig &ref)
{
    consume();
    if (current().type != Word)
    { /* TO DO: Throw an error */}
    while (current().type == Word)
    {
        ref.addIndex(consume().value);
    }
    if (current().type != Semicolon)
    { /* TO DO: Throw an error */}
    consume();
}

void Parser::parseConfigAutoIndex(AConfig &ref)
{
    consume();
    if (current().type != Word)
    { /* TO DO: Throw an error */}
    if (current().value != "on" && current().value != "off")
    { /* TO DO: Throw an error */}
    ref.setAutoindex(consume().value == "on" ? true : false);
    if (current().type != Semicolon)
    { /* TO DO: Throw an error */}
    consume();
}

void Parser::parseConfigClientMaxBodySize(AConfig &ref)
{
    (void)ref;

    consume();
    if (current().type != Word)
    { /* TO DO: Throw an error */}
    // TO DO (later): create a parseSize function/method
    if (current().type != Semicolon)
    { /* TO DO: Throw an error */}
    consume();
}

void Parser::parseConfigErrorPage(AConfig &ref)
{
    consume();
    if (current().type != Word)
    { /* TO DO: Throw an error */}
    
    int code = std::atoi(consume().value.c_str());

    if (current().type != Word)
    { /* TO DO: Throw an error */}

    std::string uri = consume().value;
    ref.addErrorPage(code, uri);

    if (current().type != Semicolon)
    { /* TO DO: Throw an error */}
    consume();
}

void Parser::parseConfigListen(ServerConfig &ref)
{
    consume();
    if (current().type != Word)
    { /* TO DO: Throw an error */}
    ref.addListen(consume().value);
    if (current().type != Semicolon)
    { /* TO DO: Throw an error */}
    consume();
}

void Parser::parseConfigServerName(ServerConfig &ref)
{
    consume();
    if (current().type != Word)
    { /* TO DO: Throw an error */}
    while (current().type == Word)
    {
        ref.addServerName(consume().value);
    }
    if (current().type != Semicolon)
    { /* TO DO: Throw an error */}
    consume();
}

// N.B: check "location ~ \.(gif|jpg|png)$"
void Parser::parseConfigMethod(LocationConfig &ref)
{
    consume();

    if (current().type != Word)
    { /* TO DO: Throw an error */}

    while (current().type == Word && (current().value == "GET"
            || current().value == "POST" || current().value == "PUT"
            || current().value == "DELETE"))
    {
        ref.addMethod(consume().value);
    }

    if (current().type != Semicolon)
    { /* TO DO: Throw an error */}
    consume();
}






/*
** ============================================================================
** Print Tokens - To delete after!
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