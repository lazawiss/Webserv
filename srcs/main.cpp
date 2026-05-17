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

#include <sys/stat.h>

#include <fstream>

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "Error: " << std::endl;
        return ERROR_ARGS;
    }

    // Step 1: Step 1: Check if the file exists
    // stat() returns 0 on success, -1 if the file does not exist
    struct stat buffer;
    int status;

    if ((status = stat(argv[1], &buffer)) != 0)
    {
        std::cout << "Error: " << std::endl;
        return ERROR;
    };

    // Step 2: Open the file and verify it is not empty
    std::ifstream file(argv[1]);
    if (!file.is_open())
    {
        std::cout << "Error: " << std::endl;
        return ERROR;
    }

    // peek() returns EOF immediately if the file contains no data
    // See: https://stackoverflow.com/questions/2390912/checking-for-an-empty-file-in-c
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        std::cout << "Error: " << std::endl;
        file.close();
        return ERROR;
    }

    // Step 3: Read and print each line of the file
    std::string line;
    while (std::getline(file, line))
        std::cout << line << "\n";

    file.close();

    return 0;
}