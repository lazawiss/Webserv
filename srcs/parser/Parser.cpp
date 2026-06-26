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
GlobalConfig parse_file(const std::string &str)
{
    std::ifstream file(str.c_str());

    if (file.is_open() == false)
        throw std::runtime_error("Error: file '" + str + "' doesn't exist");

    if (file.peek() == std::ifstream::traits_type::eof())
        throw std::runtime_error("Error: file '" + str + "' is empty");

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

    Parser parser(allTokens);
    return parser.parse();
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
// ANKIM : 1m is default
// Directive's job is just to set a ceiling on upload size; doesn't have
// ceiling of its own beyond whatever fits in the int type nginx stores in
// off_t, 64-bit which is HUGE
// edge case: setting size to 0 ? (size == 0) means disable check of clinet req bs
// size_T max SIZE_MAX
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
    char *end;
    errno = 0;

    while (i < word.size() && std::isdigit(word[i]))
        i++;
    if (i == 0)
        throw ExpectedCorrectSize();

    unsigned long value = std::strtoul(word.c_str(), &end, 10);
    if (errno == ERANGE)
        throw ExpectedLowerValue();

    if (i == word.size())
        return static_cast<size_t>(value);
    
    if (i != word.size() - 1)
        throw ExpectedCorrectSize();

    char unit = word[i];
    size_t multiply;
    if (unit == 'K' || unit == 'k')
        multiply = 1024;
    else if (unit == 'M' || unit == 'm')
        multiply = 1024 * 1024;
    else if (unit == 'G' || unit == 'g')
        multiply = 1024 * 1024 * 1024;
    else
        throw ExpectedCorrectUnit();

    if (value > ULLONG_MAX / multiply)
        throw ExpectedLowerValue();

    return (static_cast<size_t>(value) * multiply);
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

const char* Parser::ExpectedLowerValue::what() const throw()
{
    return "Invalid size value: expected at least one digit (e.g. '10M', "
        "'512K', '1G', or '1024')";
}