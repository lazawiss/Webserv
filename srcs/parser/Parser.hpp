#ifndef PARSER_HPP
# define PARSER_HPP

# include "./config/GlobalConfig.hpp"
# include "./config/AConfig.hpp"
# include "../lexer/Lexer.hpp"

# include <algorithm>
# include <climits>
# include <cmath>
# include <cstdlib>
# include <exception>
# include <fstream>
# include <iostream>
# include <sstream>
# include <stdexcept>
# include <string>
# include <sys/stat.h>
# include <vector>
# include <cstring>



GlobalConfig parse_file(const std::string& path);

/*
** ============================================================================
** Class
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
    void	applyInheritance(AConfig &child, const AConfig &parent);
    
    void	parseInheritableDirective(AConfig &ref);
    void	parseDirectiveRoot(AConfig &ref);
    void	parseDirectiveIndex(AConfig &ref);
    void	parseDirectiveAutoIndex(AConfig &ref);
    void	parseDirectiveClientMaxBodySize(AConfig &ref);
    void	parseDirectiveErrorPage(AConfig &ref);
    void	parseDirectiveListen(ServerConfig &ref);
    void	parseDirectiveServerName(ServerConfig &ref);
    void	parseDirectiveMethods(LocationConfig &ref);

    void	parseDirectiveUpload(LocationConfig &ref);
    void	parseDirectiveReturn(LocationConfig &ref);
	void	parseDirectiveCGI(LocationConfig &ref);
	void    parseSize(const std::string &word)              const;
	size_t	parseCode(const std::string &word)				const;
};

#endif