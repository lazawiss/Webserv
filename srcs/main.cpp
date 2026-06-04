/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:44:47 by lzannis           #+#    #+#             */
/*   Updated: 2026/06/04 15:19:17 by lzannis          ###   ########.fr       */
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

#define BUF_SIZE 500

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

    // TO DO: try/catch 
    
    if ((result = parse_arguments(argc)) != SUCCESS)
        return result;

    if ((result = parse_file(argv[1])) != SUCCESS)
        return result;

    Server      server;
    
    try{
        
        server.start();
        
        server.run();
    }
    catch(std::logic_error & e){
        
        std::cerr << e.what() << std::endl;
    }
  
    return 0;
}