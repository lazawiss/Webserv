# include "ListenerManager.hpp"
# include "EpollLoop.hpp"


// findCGI dans HTTPParser, si found, intitaite CGI object 
// will it poser pb for Server si on n'a pas CGI object
class CGI
{
public:
    CGI::CGI(RequestHandler const &req, ListenerManager const & listen);
    CGI(const CGI& ref);
    CGI& operator=(const CGI& ref);
    ~CGI();

// METHODS

    bool start()// Launching the fork/pipe/exec - registers pipe with epoll (code)
    void readOutput(); // called on second epoll wake; reads from pipe and append to _output
    bool isDone(); // checks to see if python cgi is good and done
    // check if child has finished 
    void    buildEnv();
    void getCGI();
    void postCGI();

    // write output to client fd

    // GETTERS
    
private:
    pid_t _pid;
    int _client_fd;
    int _pipe_fd[2];
    std::string _output;
    std::string _scriptFilename;
    std::string _fullPath;
    std::string _queryString;
    std::string _method;
    std::string _body;
    std::string _contentType;
    std::string _contentLength;
    std::string _serverName;
    std::string _serverPort;
    std::vector<std::string> _env; // owns the actual string data
    std::vector<char*>       _envp;  
};
