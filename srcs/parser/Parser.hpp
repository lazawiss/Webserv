#ifndef PARSER_HPP
# define PARSER_HPP

# include "./config/GlobalConfig.hpp"
# include "./config/AConfig.hpp"
# include "../errors/Errors.hpp"
# include "../lexer/Lexer.hpp"

# include <fstream>
# include <iostream>
# include <string>

int parse_file(const std::string& path);

/*
** ============================================================================
** Parser - Class
** ============================================================================
*/
class Parser
{
private:
    const std::vector<Token>&   _tokens;
    size_t                      _index;

public:
    Parser(const std::vector<Token> &tokens);
    ~Parser();

    const Token& current()  const;
    const Token& peek()     const;
    const Token& consume();

    GlobalConfig parse();

    void parseConfig(GlobalConfig &config);
    void parseConfigRoot(AConfig &ref);
    void parseConfigIndex(AConfig &ref);
    void parseConfigAutoIndex(AConfig &ref);
    void parseConfigClientMaxBodySize(AConfig &ref);
    void parseConfigErrorPage(AConfig &ref);

    ServerConfig parseServer();
    void parseConfigListen(ServerConfig &ref);
    void parseConfigServerName(ServerConfig &ref);
    void parseServerDirective(ServerConfig& server);
};

#endif