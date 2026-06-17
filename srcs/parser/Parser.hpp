#ifndef PARSER_HPP
# define PARSER_HPP

# include "./config/GlobalConfig.hpp"
# include "./config/AConfig.hpp"
# include "../errors/Errors.hpp"
# include "../lexer/Lexer.hpp"

# include <exception>
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

    const Token& current()      const;
    const Token& next();

    GlobalConfig parse();

    void parseConfig(GlobalConfig &config);
    void parseConfigRoot(AConfig &ref);
    void parseConfigIndex(AConfig &ref);
    void parseConfigAutoIndex(AConfig &ref);
    void parseConfigClientMaxBodySize(AConfig &ref);
    void parseConfigErrorPage(AConfig &ref);
	size_t parseSize(const std::string &word) const;

    ServerConfig parseServer();
    void parseConfigListen(ServerConfig &ref);
    void parseConfigServerName(ServerConfig &ref);
    void parseServerDirective(ServerConfig& server);

    LocationConfig parseLocation();
    void parseConfigMethod(LocationConfig &ref);
    void parseLocationDirective(LocationConfig &location);


    class NoServerDefined : public std::exception
	{
		public:
			const char* what() const throw();
	};

	class ExpectedWord : public std::exception
	{
		public:
			const char* what() const throw();
	};

    class ExpectedSemicolon : public std::exception
	{
		public:
			const char* what() const throw();
	};

    class ExpectedLBracket : public std::exception
	{
		public:
			const char* what() const throw();
	};

    class ExpectedRBracket : public std::exception
	{
		public:
			const char* what() const throw();
	};

    class UnknownDirective : public std::exception
	{
		public:
			const char* what() const throw();
	};

    class ExpectedCorrectMethod : public std::exception
	{
		public:
			const char* what() const throw();
	};

    class ExpectedCorrectAutoIndex : public std::exception
	{
		public:
			const char* what() const throw();
	};

	class ExpectedCorrectSize : public std::exception
	{
		public:
			const char* what() const throw();
	};

	class ExpectedCorrectUnit : public std::exception
	{
		public:
			const char* what() const throw();
	};
};

#endif