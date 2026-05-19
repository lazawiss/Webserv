/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:44:47 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/19 16:48:03 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors/Errors.hpp"
#include "parser/Parser.hpp"

#include <iostream>
#include <fstream>

int parse_arguments(int argc);


int parse_arguments(int argc)
{
    if (argc != 2)
    {
        std::cerr << "Error: Invalid number of arguments!" << "\n";
        std::cerr << "Usage: ./webserv ./data/<config_file>" << std::endl;
        return ERROR_ARGS;
    }
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