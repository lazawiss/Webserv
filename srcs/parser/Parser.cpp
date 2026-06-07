#include "Parser.hpp"
#include "../lexer/Lexer.hpp"

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

/*
** Opens and validates the config file, then tokenizes its content
** line by line into a single token chain.
** Returns SUCCESS if the file was parsed correctly, ERROR otherwise.
*/
std::vector<Token>   HTTPparse_file(const std::string &str)
{

    // if (file.peek() == std::ifstream::traits_type::eof())
    // {
    //     std::cerr << "Error: file '" << str << "' is empty" << std::endl;
    //     throw std::logic_error("File is empty");
    // }


    std::istringstream iss(str);
    std::string line;
    std::vector<Token> allTokens;

    while (std::getline(iss, line))
    {
        std::cout << line << "\n";
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
    

    return allTokens;
}

int parse_file(const std::string &str)
{
    std::ifstream file(str.c_str());

    if (file.is_open() == false)
    {
        std::cerr << "Error: file '" << str << "' don't exist" << std::endl;
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
        std::cout << line << "\n";
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


    return SUCCESS;
}