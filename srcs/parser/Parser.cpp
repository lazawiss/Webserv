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
** Print Tokens - To delete after!
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

void print_token_chain(const std::vector <Token> &tokens)
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

/*
** ============================================================================
** 
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
        // std::cout << line << "\n";
        
        Lexer lexer(line);
        std::vector<Token> lineTokens = lexer.tokenize();

        for (size_t i = 0; i < lineTokens.size(); i++)
        {
            if (lineTokens[i].type != End)
                allTokens.push_back(lineTokens[i]);
        }
    }

    allTokens.push_back(Token(End, ""));
    print_token_chain(allTokens);

    Parser parser(allTokens);
    GlobalConfig config = parser.parse();

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
        {
            //parseServer(config)
        }
        else if (current().type == Word)
            parseConfig(config);
        else
        { /* TO DO: Throw an error */}
    }

    if (config.getServers().empty())
    {
        // Throw error
        std::cerr << "Error: At least one server block is required" << std::endl;
    }

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