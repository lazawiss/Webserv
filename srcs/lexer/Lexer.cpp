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

static enum TokenType charToType(char c);
static bool isSign(char c);

/*
** ============================================================================
** Lexer - Constructors & Destructor
** ============================================================================
*/

Lexer::Lexer(const std::string &line) : _line(line) {}

Lexer::Lexer(const Lexer &ref) : _line(ref._line) {}

Lexer& Lexer::operator=(const Lexer &ref)
{
    if (this != &ref)
    {
        _line = ref._line;
    }
    return *this;
}

Lexer::~Lexer() {}


/*
** ============================================================================
** Lexer – Member functions
** ============================================================================
*/
 
/**
 * @brief Splits _line into a list of tokens.
 *
 * Skips whitespace, stops at '#' (line comment), emits sign tokens
 * for punctuation, and accumulates consecutive characters into Word tokens.
 *
 * @return A vector of tokens terminated by an End token.
*/
std::vector<Token> Lexer::tokenize()
{
    std::vector<Token> tokens;
    size_t i = 0;

    while (i < _line.size())
    {
        if (std::isspace(static_cast<unsigned char>(_line[i])))
        {
            i++;
        }
        else if (isSign(_line[i]))
        {
            if (_line[i] == '#')
            {
                while (i < _line.size() && _line[i] != '\n')
                    i++;
                break;
            }
            tokens.push_back(Token(charToType(_line[i]),
                std::string(1, _line[i])));

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

/**
 * @brief Maps a sign character to its corresponding TokenType.
 *
 * @param c The character to convert.
 * @return The matching TokenType, or Unknown if unrecognized.
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
/**
 * @brief Returns true if c is a recognized sign character.
 */
static bool isSign(char c)
{
    return c == '{' || c == '}' || c == ';' || c == '#';
}

