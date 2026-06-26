class CGI
{
public:
    CGI();
    ~CGI();

// METHODS
    void launch(int epoll_fd, const std::string &script_path)); // kaunching the fork/pipe/exec - registers pipe with epoll (code)
    void readOutput(); // called on second epoll wake; reads from pipe and append to _output
    bool isDone(); // checks to see if python cgi is good and done
    // check if child has finished 

    void sendResponse();
    // write output toclient fd
private:
    pid_t pid;
    int client_fd;
    int pipe_fd[2];
    std::string output;
};