#include "Parser.hpp"

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

    while (std::getline(file, line))
        std::cout << line << "\n";
        // lexer.tokenize(line);  # MARQUE
    
    // https://cplusplus.com/reference/fstream/ifstream/close/
    // No need to call file.close() explicitly.
    // "Note that any open file is automatically closed when the 
    // ifstream object is destroyed."

    return SUCCESS;
}