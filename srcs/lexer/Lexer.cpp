/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: andikim <andikim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:48:42 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/25 18:59:24 by andikim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"

/*
** ============================================================================
** Constructors & Destructor
** ============================================================================
*/

Lexer::Lexer(const std::string &line) : _line(line) {}

Lexer::Lexer(Lexer const& other) : _line(other._line) {}

Lexer& Lexer::operator=(Lexer const& other)
{
    if (this != &other)
    {
        _line = other._line;
    }
    return *this;
}

Lexer::~Lexer() {}

/*
** ============================================================================
** Static helpers (only visible in this file)
** ============================================================================
*/

static enum TokenType charToType(char c)
{
    switch (c)
    {
        case '{': return LBracket;
        case '}': return RBracket;
        case ';': return Semicolon;
        case '#': return Hashtag;

        default:  return Unknown;
    }
}

static bool isSign(char c)
{
    return c == '{' || c == '}' || c == ';' || c == '#';
}

/*
** ============================================================================
** Tokenize
** ============================================================================
*/
 
/*
** Splits _line into a list of tokens.
** Three cases at each character:
**   1. whitespace  -> skip
**   2. sign        -> push a sign token
**   3. other       -> accumulate until next whitespace or sign -> Word token
** An End token is always appended at the end of the vector.
*/
std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < _line.size())
    {
        if (std::isspace(static_cast<unsigned char>(_line[i])))
            i++;
        else if (isSign(_line[i]))
        {
            tokens.push_back(Token(charToType(_line[i]), std::string(1, _line[i])));
            i++;
        }
        else
        {
            size_t start = i;

            while (i < _line.size() && !isSign(_line[i])
                   && !std::isspace(static_cast<unsigned char>(_line[i])))
                i++;
            tokens.push_back(Token(Word, _line.substr(start, i - start)));
        }
    }

    tokens.push_back(Token(End, ""));
    return tokens;
}
