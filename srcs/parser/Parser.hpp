#ifndef PARSER_HPP
# define PARSER_HPP

# include "../errors/Errors.hpp"
# include "../lexer/Lexer.hpp"

# include <fstream>
# include <iostream>
# include <string>

int parse_file(const std::string& path);

#endif