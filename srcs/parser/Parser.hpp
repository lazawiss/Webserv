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

	// ── Orthodox canonical form (no copy) ───────────────────────────────────
	// Copy constructor and assignment are declared private and never
	// defined, since Parser holds a reference that cannot be reassigned.
	Parser(const Parser &ref);
    Parser& operator=(const Parser &ref);

public:
    // ── Orthodox canonical form ─────────────────────────────────────────────
    Parser(const std::vector<Token> &tokens);
    ~Parser();

	// ── Recursive descent parser ────────────────────────────────────────────
    GlobalConfig	parse();
    ServerConfig	parseServer();
    LocationConfig	parseLocation();

	// ── Token navigation methods ─────────────────────────────────────────────
    const Token& current()      const;
    const Token& next();

	// ── Member methods ──────────────────────────────────────────────────────
    void	parseInheritableDirective(AConfig &ref);
    void	parseDirectiveRoot(AConfig &ref);
    void	parseDirectiveIndex(AConfig &ref);
    void	parseDirectiveAutoIndex(AConfig &ref);
    void	parseDirectiveClientMaxBodySize(AConfig &ref);
    void	parseDirectiveErrorPage(AConfig &ref);
    void	parseDirectiveListen(ServerConfig &ref);
    void	parseDirectiveServerName(ServerConfig &ref);
    void	parseDirectiveMethods(LocationConfig &ref);
	size_t	parseSize(const std::string &word)				const;
	size_t	parseCode(const std::string &word)				const;

    // ── Throw errors ────────────────────────────────────────────────────────
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

	class ExpectedLowerValue : public std::exception
	{
		public:
			const char* what() const throw();
	}
};

#endif