/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:45:54 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/05 17:49:42 by lzannis          ###   ########.fr       */
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
    End
} TokenType;

struct Token
{
    std::string value;
    TokenType type;
};

class Lexer
{
  
private:
public:

    Lexer();
    Lexer( Lexer const & other );
    ~Lexer();
    Lexer & operator=( Lexer const & other);

};

#endif