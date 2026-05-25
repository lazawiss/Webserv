#include "Parser.hpp"
#include "../lexer/Lexer.hpp"

static std::string tokenTypeToString(TokenType type)
{
    switch(type)
    {
        case Word: return "Word";
        case LBracket: return "LBracket";
        case RBracket: return "RBracket";
        case Semicolon: return "Semicolon";
        case Unknown: return "Unknown";
        case Hashtag: return "Hashtag";
        case End: return "End";
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

int parse_file(const std::string &str)
{
    // .c_str() is required in C++98 because std::ifstream's constructor
    // only accepts const char*, not std::string (fixed in C++11)
    std::ifstream file(str.c_str());

    if (file.is_open() == false)
    {
        std::cerr << "Error: file '" << str << "' don't exist" << std::endl;
        return ERROR;
    }

    // peek() returns EOF immediately if the file contains no data
    // https://stackoverflow.com/questions/2390912/checking-for-an-empty-file-in-c
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
        std::vector<Token> lineTokens = lexer.tokenize(line);

        for (size_t i = 0; i < lineTokens.size(); i++)
        {
            if (lineTokens[i].type != End)
                allTokens.push_back(lineTokens[i]);
        }
    }
    allTokens.push_back(Token(End, ""));
    print_token_chain(allTokens);
    
    // https://cplusplus.com/reference/fstream/ifstream/close/
    // No need to call file.close() explicitly.
    // "Note that any open file is automatically closed when the 
    // ifstream object is destroyed."

    return SUCCESS;
}