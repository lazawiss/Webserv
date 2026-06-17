#ifndef ACONFIG_HPP
# define ACONFIG_HPP

# include <iostream>
# include <string>
# include <map>
# include <vector>

/*
** ============================================================================
** AConfig - Class
** ============================================================================
*/
class AConfig
{
private:
    std::string                 _root;
    std::vector<std::string>    _index;
    std::map<int, std::string>  _error_pages;
    bool                        _autoindex;
    size_t                      _client_max_body_size;

public:
    // ── Orthodox canonical form ─────────────────────────────────────────────
    AConfig();
    AConfig(const AConfig &ref);
    AConfig& operator=(const AConfig &ref);
    virtual ~AConfig() = 0;

    // ── Getters ─────────────────────────────────────────────────────────────
    const std::string&                    getRoot()              const;
    const std::vector<std::string>&       getIndex()             const;
    const std::map<int, std::string>&     getErrorPages()        const;
    bool                                  getAutoindex()         const;
    size_t                                getClientMaxBodySize() const;

    // ── Setters ─────────────────────────────────────────────────────────────
    void setRoot(const std::string &root);
    void addIndex(const std::string &index);
    void setAutoindex(bool autoindex);
    void setClientMaxBodySize(size_t size);
    void addErrorPage(int code, const std::string &uri);
};

#endif