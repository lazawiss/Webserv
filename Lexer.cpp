/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Lexer.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lzannis <lzannis@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/05 17:48:42 by lzannis           #+#    #+#             */
/*   Updated: 2026/05/05 17:48:56 by lzannis          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Lexer.hpp"

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

std::vector<std::vector> first(ifstream file){
    std::vector configFile;

    // read each line via getline 
    // send to tokenize
    // add each newly tokenized to vector configFile;
    return (configFile);
}

std::vector<Token> tokenize(std::string &linefromSourceCode){ // returns a vector of tokens
    std::vector<Token> tokens;
    std::vector<std::string> src = split(lineFromSourceCode);
    while (!src.empty()){
        // take each "words" and categorize them;
        // are they words? are they ";" what are they?
        // note: check to make sure that all pieces of config file are resumed in enum
        // note2: make sure that config file does not have other separators without meaning
        // note3: how many spaces can we have between elements for it to still be valid? only 0 or
        // unlimited?
    }
    return tokens; 
}

std::vector<std::string> split(const std::string &lineFromSourceCode){
    std::vector<std::string> words; // vector that dynamically adds words
    // look for the space where there is a " " and then have each strchr string
    return words;
}

// https://medium.com/@tharunappu2004/writing-a-lexer-in-c-a-step-by-step-guide-a1d5c55ac04d
