# include "ListenerManager.hpp"
# include "EpollLoop.hpp"


// findCGI dans HTTPParser, si found, intitaite CGI object 
// will it poser pb for Server si on n'a pas CGI object
class CGI
{
public:
    CGI();
    ~CGI();

// METHODS
    bool isValidCGI()
    void launch(int epoll_fd, const std::string &script_path); // Launching the fork/pipe/exec - registers pipe with epoll (code)
    void readOutput(); // called on second epoll wake; reads from pipe and append to _output
    bool isDone(); // checks to see if python cgi is good and done
    // check if child has finished 

    void sendResponse();
    // write output to client fd
private:
    pid_t pid;
    int client_fd;
    int pipe_fd[2];
    std::string output;
};
