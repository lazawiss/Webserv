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

/**
 * @section Constructors & Destructors
 * ---------------------------------------------------------------------------
*/

Lexer::Lexer(const std::string& text) : input(text), position(0) {}

Lexer::Lexer(Lexer const& other) : input(other.input), position(other.position)
{}

Lexer& Lexer::operator=(Lexer const& other){
    if (this != &other)
    {
        input = other.input;
        position = other.position;
    }
    return *this;
}

Lexer::~Lexer() {}


// Lexer should:
// recognize and categorize various elements in source code
// RETURNS VECTOR OF TOKENS

// each line should be checked by getline and then used to split up all characters;
// check by looking at the iterator where the next separator is
// each cut string (strchr???) can then be something and we classify
// do this until the config file is done

// when file is read, we do a get next line to it
// each line goes through tokenize
// when it returns to the og fucntion (first)

// std::vector<std::vector> first(std::ifstream file)
// {
//     std::vector configFile;

//     // read each line via getline 
//     // send to tokenize
//     // add each newly tokenized to vector configFile;
//     return (configFile);
// }


// MARQUE DELPHINE
// This function is no longer needed in Lexer.cpp ⬆️.
// getline() is now called directly in Parser.cpp, which also handles
// the call to tokenize(). Feel free to add more error types to the
// enum, such as ERROR_LEXER.

// RESPONSE ANDI
// No need for error no? Because all will be taken by lexer?

/**
 * @brief Splits a line of source code into a vector of tokens.
 * ---------------------------------------------------------------------------
 * 
 * 
 */

static bool isSign(char c)
{
    return c == '{' || c == '}' || c == ';' || c == '#';
}

static TokenType charToType(char c)
{
    switch (c)
    {
        case '{' : return LBracket;
        case '}': return RBracket;
        case ';': return Semicolon;
        case '#': return Hashtag;
        default: return Unknown;
    }
}
 
std::vector<Token> Lexer::tokenize(std::string &line)
{
    std::vector<Token> tokens;
    size_t pos = 0;

    while (pos < line.size())
    {
        if (std::isspace(line[pos]))
        {
            pos++;
            continue;
        }
        if (!isSign(line[pos]) && !std::isspace(line[pos]))
        {
            std::string word;
            while (pos < line.size() && !isSign(line[pos]) && !std::isspace(line[pos]))
            {
                word += line[pos];
                pos++;
            }
            tokens.push_back(Token(Word, word));
            continue;
        }
        tokens.push_back(Token(charToType(line[pos]), std::string(1, line[pos])));
        pos++;
    }
    tokens.push_back(Token(End, ""));
    return tokens;
}

/**
 * @brief
 * ---------------------------------------------------------------------------
 * 
 * 
 */
// std::vector<std::string> split(const std::string &lineFromSourceCode)
// {
//     std::vector<std::string> words; // vector that dynamically adds words
//     // look for the space where there is a " " and then have each strchr string
//     return words;
// }

// https://medium.com/@tharunappu2004/writing-a-lexer-in-c-a-step-by-step-guide-a1d5c55ac04d
