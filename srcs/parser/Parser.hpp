#ifndef PARSER_HPP
# define PARSER_HPP

# include "../errors/Errors.hpp"
# include "../lexer/Lexer.hpp"


# include <fstream>
# include <iostream>
# include <sstream>
# include <string>
# include <vector>
# include <algorithm>
# include <stdexcept>

int parse_file(const std::string& path);

std::vector<Token>  HTTPparse_file(const std::string& path);

//create class Parser ? 
#endif