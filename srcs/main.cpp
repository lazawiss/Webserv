/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:44:47 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/05 17:49:09 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

// #include "Lexer.hpp"

#include "errors/Errors.hpp"
#include <iostream>

#include <fstream>

int parse_arguments(int argc)
{
    if (argc != 2)
    {
        std::cerr << "Error:" << std::endl;
        return ERROR_ARGS;
    }
    return SUCCESS;
}

int parse_file(const std::string &str)
{
    // .c_str() is required in C++98 because std::ifstream's constructor
    // only accepts const char*, not std::string (fixed in C++11)
    std::ifstream file(str.c_str());

    if (file.is_open() == false)
    {
        std::cerr << "Error: file don't exist" << std::endl;
        return ERROR;
    }

    // peek() returns EOF immediately if the file contains no data
    // https://stackoverflow.com/questions/2390912/checking-for-an-empty-file-in-c
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        std::cerr << "Error: file is empty" << std::endl;
        return ERROR;
    }

    std::string line;

    while (std::getline(file, line))
        std::cout << line << "\n";
    
    // https://cplusplus.com/reference/fstream/ifstream/close/
    // No need to call file.close() explicitly.
    // "Note that any open file is automatically closed when the 
    // ifstream object is destroyed."

    return SUCCESS;
}

int main(int argc, char **argv)
{
    int result;

    // TO DO: try/catch 
    if ((result = parse_arguments(argc)) != SUCCESS)
		return result;

    if ((result = parse_file(argv[1])) != SUCCESS)
        return result;

    return 0;
}