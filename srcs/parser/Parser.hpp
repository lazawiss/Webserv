#ifndef PARSER_HPP
# define PARSER_HPP

# include "../errors/Errors.hpp"
# include "../lexer/Lexer.hpp"

# include <fstream>
# include <iostream>
# include <string>

class Parser {
// lire vector
// est-ce qu'on est un directive ou bien un bloc?
// est-ce que j'ai fini ce contexte?
public:
    Parser(const std::vector<Token> &list, bool err);
    Parser& operator=(Parser const &existing);
    Parser (Parser const &existing);
    ~Parser();
private:
    const std::vector<Token> _list;
    std::vector<Token>::const_iterator _it;
    bool _err;
};

class Config_Abstract { 
// all three, general server and location are all the same
// root 
// index
 };
class Config_General : public Config_Abstract {};
class Config_Server : public Config_Abstract {
//nombre de servers mais si on fait abstract on pourrait pas compter depuis base
//config_location 
// parser donne "server block" et ici on parse les infos relative aux serveurs
// until the crochet right to close server block 
};
class Config_Location : public Config_Abstract {
//nombre de locations

};

// pour la partie l'exec, on prend les classes instances de parser
// lea prend pour exec, au moment ou elle instantie les classes SERVER EXEC
// afin d'avoir les informations deja propre et claires pour les sockets 
// put it in the class of exec server - 
// getter setter
// question pour server : comment on fait si on des servers multiples ? mais avec aussi
// les clients multiples? jo dit faut les faire donc exec clean, on envoie les servers config en appellant lea 
// truc
// 

int parse_file(const std::string& path);

#endif