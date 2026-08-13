#include "Parser.hpp"

#include "../lexer/Lexer.hpp"

/*
** ============================================================================
** Orthodox canonical form
** ============================================================================
*/

Parser::Parser(const std::vector<Token> &tokens) : _tokens(tokens), _index(0) {}

Parser::~Parser() {}

/*
** ============================================================================
** Token navigation methods
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
** Parse file
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
        throw std::runtime_error(" file '" + str + "' is empty");

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
        {
            ServerConfig server = parseServer();

            applyInheritance(server, config);
            std::vector<LocationConfig>& locs = server.getLocations();
            for (size_t i = 0; i < locs.size(); i++)
                applyInheritance(locs[i], server);

            config.addServer(server);
        }
        else if (current().type == Word)
            parseInheritableDirective(config);
        else
            throw std::runtime_error("Unexpected token '" +  current().value
                + "', should be a 'word' type");
    }

    if (config.getServers().empty())
        throw std::runtime_error("At least one server is required");

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
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'left braket' type");

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
            throw std::runtime_error("Unexpected token '" +  current().value
                + "', should be a 'word' type");
    }
    
    if (current().type != RBracket)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'right braket' type");

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
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");

    location.setPath(next().value);

    if (current().type != LBracket)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'left braket' type");

    next();

    while (current().type != RBracket && current().type != End)
    {
        if (current().type == Word && current().value == "methods")
            parseDirectiveMethods(location);
        else if (current().type == Word && current().value == "upload")
            parseDirectiveUpload(location);
        else if (current().type == Word && current().value == "return")
            parseDirectiveReturn(location);
        else if (current().type == Word && current().value == "cgi_extension")
            parseDirectiveCGI(location);
        else if (current().type == Word)
            parseInheritableDirective(location);
        else
            throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");
    }
    
    if (current().type != RBracket)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'right braket' type");

    next();

    return location;
}

void Parser::applyInheritance(AConfig &child, const AConfig &parent)
{
    // Root implementation
    if (child.getRoot().empty() && !parent.getRoot().empty())
        child.setRoot(parent.getRoot());
    // Client Max Size Body implementation
    if (child.getClientMaxBodySize().empty() &&
        !parent.getClientMaxBodySize().empty())
            child.setClientMaxBodySize(parent.getClientMaxBodySize());
    // Index implementation
    if (child.getIndex().empty() && !parent.getIndex().empty())
    {
        const std::vector<std::string> &index = parent.getIndex();
        for (size_t i = 0; i < index.size(); i++)
            child.addIndex(index[i]);
    }
    // AutoIndex implementation
    if (child.getAutoindex().empty() && !parent.getAutoindex().empty())
        child.setAutoindex(parent.getAutoindex());
    // ErrorPages implementation
    const std::map<int, std::string> &parentErrors = parent.getErrorPages();
    for (std::map<int, std::string>::const_iterator it = parentErrors.begin();
        it != parentErrors.end(); ++it)
    {
        if (child.getErrorPages().find(it->first) == child.getErrorPages().end())
            child.addErrorPage(it->first, it->second);
    }
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
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', unknown directive in global, server or location context");
}

/*
** ============================================================================
** Member methods
** ============================================================================
*/

void Parser::parseDirectiveRoot(AConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");

    std::string path = current().value;

    struct stat info;
    if (stat(path.c_str(), &info) != 0)
        throw std::runtime_error("Root path does not exist: '" + path + "'");
    if (!S_ISDIR(info.st_mode))
        throw std::runtime_error("Root path isn't a directory: '" + path + "'");

    if (!ref.getRoot().empty())
        throw std::runtime_error("Duplicate 'root' directive");

    ref.setRoot(next().value);

    if (current().type != Semicolon)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");

    next();
}

void Parser::parseDirectiveIndex(AConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");

    if (!ref.getIndex().empty())
        throw std::runtime_error("Duplicate 'index' directive");

    while (current().type == Word)
    {
        ref.addIndex(current().value);
        next();
    }

    if (current().type != Semicolon)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");

    next();
}

void Parser::parseDirectiveAutoIndex(AConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");

    if (current().value != "on" && current().value != "off")
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', auto index can be 'on' or 'off'");

    if (!ref.getAutoindex().empty())
        throw std::runtime_error("Duplicate 'autoindex' directive");

    ref.setAutoindex(next().value);

    if (current().type != Semicolon)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");

    next();
}

void Parser::parseDirectiveClientMaxBodySize(AConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");

    if (!ref.getClientMaxBodySize().empty())
        throw std::runtime_error("Duplicate 'client_max_body_size' directive");

    parseSize(current().value);
    ref.setClientMaxBodySize(next().value);

    if (current().type != Semicolon)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");

    next();
}

void Parser::parseSize(const std::string &word) const
{
    size_t i = 0;

    while (i < word.size() && std::isdigit(word[i]))
        i++;
    if (i == 0)
        throw std::runtime_error("Invalid size value (e.g. '10M', "
            "'512K', '1G', or '1024')");

    if (i == word.size())
        return;
    
    if (i != word.size() - 1)
        throw std::runtime_error("Invalid size value: expected at least "
            "one digit (e.g. '10M', '512K', '1G', or '1024')");

    char unit = word[i];
    if (unit != 'K' && unit != 'k' && unit != 'M' && unit != 'm' 
        && unit != 'G' && unit != 'g')
            throw std::runtime_error("Invalid size unit: expected 'K', 'M' or "
            "'G' after the number (e.g. '10M', '512K', '1G')");
}

void Parser::parseDirectiveErrorPage(AConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");

    int code = parseCode(next().value);

    if (code != 400 && code != 404 && code != 405 && code != 413 && code != 414
        && code != 421 && code != 500 && code != 502)
    {
        std::ostringstream oss;
        oss << code;
        throw std::runtime_error("Invalid HTTP error code '" + oss.str()
            + "', accepted values: 400, 404, 405, 413, 414, 421, 500, 502");
    }

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");

    std::string uri = next().value;
    ref.addErrorPage(code, uri);

    if (current().type != Semicolon)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");

    next();
}

size_t Parser::parseCode(const std::string &word) const
{
    if (word.size() != 3)
        throw std::runtime_error("Invalid HTTP error code: expected a "
            "value between 400 and 599 (e.g. '404', '500')");

    for (size_t i = 0; i < word.size(); i++)
    {
        if (!std::isdigit(word[i]))
            throw std::runtime_error("Invalid HTTP error code: expected a "
                "value between 400 and 599 (e.g. '404', '500')");
    }

    return std::atoi(word.c_str());
}

void Parser::parseDirectiveListen(ServerConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");

    const std::string listen = current().value;
    size_t nbr = listen.find(':');
    if (nbr == std::string::npos || nbr == 0 || nbr == listen.size() - 1)
        throw std::runtime_error("Invalid listen directive: " + listen);
    
    std::string str = listen.substr(nbr + 1);
    for (size_t i = 0; i < str.size(); i++)
        if (!std::isdigit(str[i]))
            throw std::runtime_error("Invalid port in listen directive");
    
    int port = std::atoi(str.c_str());
    if (port < 1 || port > 65535)
        throw std::runtime_error("Port out of range in listen directive");

    if (!ref.getListen().empty())
        throw std::runtime_error("Duplicate 'listen' directive");

    ref.setListen(next().value);

    if (current().type != Semicolon)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");

    next();
}

void Parser::parseDirectiveServerName(ServerConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");
    
    while (current().type == Word)
    {
        ref.addServerName(current().value);
        next();
    }

    if (current().type != Semicolon)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");

    next();
}

void Parser::parseDirectiveMethods(LocationConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");

    if (!ref.getMethods().empty())
        throw std::runtime_error("Duplicate 'methods' directive");

    while (current().type == Word)
    {
        if (current().value != "GET" && current().value != "POST"
            && current().value != "DELETE")
                throw std::runtime_error("Invalid HTTP method, should be 'GET'"
                    ", 'POST' or 'DELETE'");

        ref.addMethod(next().value);
    }

    if (current().type != Semicolon)
       throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");

    next();
}

void Parser::parseDirectiveUpload(LocationConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");
    
    std::string path = current().value;

    struct stat info;
    if (stat(path.c_str(), &info) != 0)
        throw std::runtime_error("Upload path does not exist: '" + path + "'");
    if (!S_ISDIR(info.st_mode))
        throw std::runtime_error("Upload path isn't a directory: '" + path + "'");
    
    if (!ref.getUpload().empty())
        throw std::runtime_error("Duplicate 'upload' directive");

    ref.setUpload(next().value);

    if (current().type != Semicolon)
       throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");

    next();
}

void Parser::parseDirectiveReturn(LocationConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");
        
    if (!ref.getReturn().empty())
        throw std::runtime_error("Duplicate 'return' directive");

    int code = parseCode(next().value);

    if (code != 400 && code != 404 && code != 405 && code != 413 && code != 414
        && code != 421 && code != 500 && code != 502)
    {
        std::ostringstream oss;
        oss << code;
        throw std::runtime_error("Invalid HTTP error code '" + oss.str()
            + "', accepted values: 400, 404, 405, 413, 414, 421, 500, 502");
    }

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");

    std::string uri = next().value;
    ref.addErrorPage(code, uri);

    if (current().type != Semicolon)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");

    next();
}

void Parser::parseDirectiveCGI(LocationConfig &ref)
{
    next();

    if (current().type != Word)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'word' type");


    if (current().value != ".py" && current().value != ".php")
        throw std::runtime_error("Invalid CGI file format: "
            "expected .py or .php");

    std::string key = current().value;

    next();

    std::string path = current().value;

    struct stat info;
    if (stat(path.c_str(), &info) != 0)
        throw std::runtime_error("CGI interpreter does not exist: '" + path + "'");
    if (!S_ISREG(info.st_mode))
        throw std::runtime_error("CGI interpreter path is not a regular file: '" + path + "'");

    ref.addMap(key, path);

    next();

    if (current().type != Semicolon)
        throw std::runtime_error("Unexpected token '" +  current().value
            + "', should be a 'semi colon' type");
        
    next();
}
