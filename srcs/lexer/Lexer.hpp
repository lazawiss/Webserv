/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andikim <andikim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:45:54 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/25 17:45:42 by andikim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LEXER_HPP
# define LEXER_HPP

# include <fstream>
# include <iostream>
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

}   TokenType;

struct Token
{
    TokenType   type;
    std::string value;

    Token (TokenType t, const std::string& v): type(t), value(v) {};
};

class Lexer
{
private:
    std::string _line;

public:
    Lexer(const std::string &line);

    Lexer(Lexer const &ref);
    Lexer& operator=(Lexer const &ref);
    ~Lexer();

    std::vector<Token> tokenize();
};

#endif