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

    void parseInheritableDirective(AConfig &ref);
    void parseDirectiveRoot(AConfig &ref);
    void parseDirectiveIndex(AConfig &ref);
    void parseDirectiveAutoIndex(AConfig &ref);
    void parseDirectiveClientMaxBodySize(AConfig &ref);
    void parseDirectiveErrorPage(AConfig &ref);
	size_t parseSize(const std::string &word) const;
	size_t parseCode(const std::string &word) const;

    ServerConfig parseServer();
    void parseDirectiveListen(ServerConfig &ref);
    void parseDirectiveServerName(ServerConfig &ref);

    LocationConfig parseLocation();
    void parseDirectiveMethods(LocationConfig &ref);


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

	class ExpectedCorrectCode: public std::exception
	{
		public:
			const char* what() const throw();
	};
};

#endif