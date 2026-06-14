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
** 
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

// bool precheck_token_chain(const std::vector<Token> &tokens)
// {
//     int server = 0;
//     int b = 0;
//     if (tokens.size() == 0)
//         return (false);
//     std::vector<Token>::const_iterator it = tokens.begin();
//     if (!tokens[tokens.size() - 1].type == End)
//         return (false);
//     while (it != tokens.end())
//     {
//         if (it->type == LBracket)
//             b++;
//         if (it->type == RBracket)
//             b--;
//         if (it->type == Word && (it->value == "server"))
//         {
//             if (it + 1 != tokens.end())
//             {
//                 if ((it + 1)->type == LBracket)
//                     server++;
//             }
//         }
//         it++;
//     }
//     if (server == 1 || (b != 0))
//         return false;
//     return true;
// }

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

    // if (precheck_token_chain(allTokens) == false)
    // {
    //     std::cerr << "Error: file '" << str << "' does not have server, end, or correct brackets" << std::endl;
    //     return ERR_PREVALIDATION_CONFIG;
    // }
    return SUCCESS;
}