/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:44:47 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/23 13:37:51 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "errors/Errors.hpp"
#include "parser/Parser.hpp"
#include "server/Server.hpp"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <netdb.h>
#include <cstring>
#include <string>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <csignal>
#include <cerrno>
#include <cstdlib>
#include <ctime>
#include <cstdio>

#define BUF_SIZE 800000

int parse_arguments(int argc);

int parse_arguments(int argc)
{
    if (argc != 2)
    {
        std::cerr << "Error: Invalid number of arguments!" << "\n";
        std::cerr << "Usage: ./webserv ./data/config/<config_file>" << std::endl;
        return ERROR_ARGS;
    }
    return SUCCESS;
}
int main(int argc, char **argv)
{
    int             result;
    
    if ((result = parse_arguments(argc)) != SUCCESS)
        return result;

    try
    {
        GlobalConfig config;
        
        config = parse_file(argv[1]);
        Server server(config);
  
        server.start();
        server.run();
    }
    catch(const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return ERROR;
    }
  
    return 0;
}