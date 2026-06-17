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

/*
** ============================================================================
** Lexer - Enums & structs
** ============================================================================
*/

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

/*
** ============================================================================
** Lexer - Class
** ============================================================================
*/

class Lexer
{
private:
    std::string _line;

public:
    // ── Orthodox canonical form ───────────────────────────
    Lexer(const std::string &line);
    Lexer(Lexer const &ref);
    Lexer& operator=(Lexer const &ref);
    ~Lexer();

    // ── Member methods ────────────────────────────────────
    std::vector<Token> tokenize();
};

#endif