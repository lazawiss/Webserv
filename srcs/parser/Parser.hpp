#ifndef PARSER_HPP
# define PARSER_HPP

# include "../errors/Errors.hpp"
# include "../lexer/Lexer.hpp"

# include <fstream>
# include <iostream>
# include <string>

int parse_file(const std::string& path);

class Parser
{
private:
    const std::vector<Token>&   _tokens;
    size_t                      _index;

public:
    Parser(const std::vector<Token> &tokens);
    ~Parser();

    const Token& current()  const;
    const Token& peek()     const;
    const Token& consume();
};

#endif