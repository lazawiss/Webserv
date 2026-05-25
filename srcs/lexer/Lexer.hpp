/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andikim <andikim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:45:54 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/25 15:19:17 by andikim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
# define LEXER_HPP

# include <fstream>
# include <iostream>
# include <list>
# include <map>
# include <stdexcept>
# include <string>
# include <vector>

typedef enum TokenType
{
    Word,
    RBracket,
    LBracket,
    Semicolon,
    Hashtag,
    Unknown,
    End
} TokenType;

struct Token // struct is like class but always public in C++
{
    std::string value;
    TokenType type;
};

class Lexer
{
private:
    std::string input; 
    size_t position;

    public:
    Lexer(const std::string& text);
    Lexer(Lexer const& other);
    ~Lexer();
    Lexer& operator=(Lexer const& other);
    std::vector<Token> tokenize(std::string &line);

};

#endif