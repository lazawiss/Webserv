*This project has been created as part of the 42 curriculum by ankim, dpaiva, lzannis.*

# Webserv

## Description
**Webserv** is an HTTP server, written in **C++98**. The goal of this project is to create a web server, heavily inspired by the very popular web server, **NGINX**; its main role is to receive HTTP requests of clients and respond to it. NGINX serves as the model of reference for the general behavior of this project.

Webserv runs based on the following functions :
- read a **configuration file**, with a syntax inspired of that which could be found on NGINX.
- create one or several **listening sockets** (a pair address:port is assigned to each server);
- handle **several clients simultaneously**, in a **non-blocking** manner;
- **parse** received HTTP requests and respond with the **GET**, **POST**, and **DELETE** methods;
- serve **static files**, handling the **upload** function of files,
the **listing of the directory** (autoindex), **redirections**
and personalized **error pages**;
- be able to execute CGI scripts (written in either Python or PHP), based on the extension of the requested file.

## Instructions

### Prerequisites

- A compiler C++ that is compatible **C++98** (`g++` or `clang++`)
- A **Linux** system (the event loop is carried by `epoll`, a Linux kernel function)
- **Python 3** and/or **PHP-CGI** if we want to be access CGI functionalities. 

### Compilation
```bash
git clone [url]
cd webserv
make
```

Compiling generates a runnable program,`webserv` at the root of the project. The `Makefile` additionally enables the following functions: 
```bash
make clean          # delete all the object files
make fclean         # delete all object files as well as the program
make re             # recompiles entirely the project
```

### Execution

```bash
./webserv [fichier de configuration]
```

A configuration file must be passed as an argument. Examples are available in `data/config/`.

### Configuration syntax

The configuration follows a syntax inspired by NGINX, on three different levels: `global`, `server` and `location`.

```nginx
client_max_body_size 10M;

server {
    listen       127.0.0.1:8080;
    server_name  tsuki;

    location / {
        root        data/www/tsuki;
        index       index.html;
        methods     GET;
        autoindex   off;
    }

    location /upload {
        root        data/upload;
        index       index.html;
        methods     GET POST DELETE;
        autoindex   off;
    }

    llocation /cgi-bin/python {
        root            data/cgi-bin;
        methods         POST;
        cgi_extension   .py  /usr/bin/python3;
        index           index.py;
    }
}
```

**Available directives :**
| Directive              | Context                          | Description                            | 
|------------------------|----------------------------------|----------------------------------------|
| `client_max_body_size` | `global` / `server` / `location` | Max size of the request body           |
| `root`                 | `global` / `server` / `location` | Root directory of the files served     |
| `index`                | `global` / `server` / `location` | Default file served                    |
| `error_page`           | `global` / `server` / `location` | Personalized error pages               |
| `autoindex`            | `global` / `server` / `location` | Activation for directory listing       |
| `listen`               | `server`                         | IP address and the listening port      |
| `server_name`          | `server`                         | Domain name (virtual hosting)          |
| `methods`              | `location`                       | Authorized HTTP methods                | 
| `return`               | `location`                       | HTTP Redirections                      |
| `cgi_extension`        | `location`                       | Extension specifications → for CGIs    |


### Tester le serveur

Once the program is running, the server listens on the addresses/ ports definied in the configuration file. It can be tested via : 

- with a web browser :

```bash
http://localhost:8080
````

- with `curl` :

```bash
curl -v http://127.0.0.1:8080/
```
- with telnet :
```bash
telnet localhost 8080
```

- with a stress test tool (ex : `siege`) in order to verify the stability of the server under load.
```bash
siege -c 10 -t 1M http://127.0.0.1:8080/
```

## Architecture

**Architecture of projet :**

```
Lexer / Parser      →  reads the config file  →  GlobalConfig / ServerConfig / LocationConfig

Server
├── SignalManager       handles SIGINT / SIGQUIT
├── ListenerManager     listening sockets (one per `listen`)
├── EpollLoop           our main loop (epoll)
│   ├── HTTPParser      parses the raw request
│   ├── RequestHandler  constructs the response
│   └── CGIHandler      executes the Python / PHP scripts
```

**The flow of a request :**

```
Client
  │  TCP connect
  ▼
ListenerManager  ──accept()──►  new fd client
  │
  ▼
EpollLoop  (epoll_wait)
  │
  ├─ EPOLLIN  ──►  HTTPParser       parses methods / headers / body
  │                    │
  │                    ▼
  │               RequestHandler    decides on the response
  │                    ├── static files      →  read from disk
  │                    ├── upload            →  write from disk
  │                    └── CGI (.py/.php)    →  CGIHandler (fork + pipe)
  │
  └─ EPOLLOUT ──►  send the response  ──►  Client
```

## Vocabulary

### Client / Server
A **server** is a program that waits for connections (HTTP requests) and responds to the different asks. A **client** (browser, `curl`, etc.) initiates the connection and sends an HTTP request. The communications passes by a TCP/IP network : the client opens the connection towards the IP address and the port of the server. 

### HTTP (HyperText Transfer Protocol)
The text protocol on top of TCP that defines the shape of exchanges between client and server. An **HTTP request** contains:
- a **request line**: with method + the path + the version (`GET /index.html HTTP/1.1`) 
- **headers**: metadata (`Host:`, `Content-Type:`, `Content-Length:`…)
- a **body** (optional) : sent data (forms, uploaded files)

A **response** contains a status code (`200 OK`, `404 Not Found`…), headers, and the content.

### Socket
Network entry point represented by a file descriptor (fd). The server creates a listening socket via directive `listen`, accepts the incoming connections (`accept()`), then each client acquires its own fd to read/ write.

### HTTP Methods
- **GET** : request a resource (page, image…)
- **POST** : send data to the server (forms, upload)
- **DELETE** : delete a resource

### epoll()
Linux table that keeps an eye out on several fds silmultaneously **without blocking**. Instead of waiting on a single fd, `epoll_wait()` returns a list of fds that are ready to read or to write. It is what allows us to handle dozens of clients in parallel of each other, in a **single thread**, with creating a thread per connection.

### Non-blocking (non-blocking I/O)
By default, a `read()` ou `write()` call waits for the data to be available. In the non-blocking mode, it returns immediately if nothing is ready. Coupled with epoll, we never wait for a slow client that would block.

### CGI (Common Gateway Interface)
The standard interface for a web server to excute an external script (Python, PHP..). The server creates a child process via `fork()`, sends him the request via the environment variables and via `execve()` + pipes, the response is generated and readable on stdout. In our case, it is `CGIHandler` that orchestrates this.

### Parsing / Lexer / Parser
Transform raw text into structured data. The **lexer** cuts the config file into tokens. The **Parser** reads these tokens and constructs the objects `GlobalConfig` / `ServerConfig` / `LocationConfig`. The same pricinipal for `HTTPParser`, which are for raw HTTP requests. 

### Virtual hosting
Host several sites on the same server (same IP/port) by distinguishing via the header `Host`. Each `server` block with a different `server_name` corresponds to its own unique site. 

## Resources

### Documentation and articles

- [Official NGINX Documentation](https://nginx.org/en/docs/) — reference model for the syntax of the configuration and the general behavior of server
- [RFC 9110 – HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110.html)
- [RFC 9112 – HTTP/1.1](https://www.rfc-editor.org/rfc/rfc9112.html)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — classic reference for socket programming in C/C++
- [man epoll(7)](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [The Common Gateway Interface (CGI) — RFC 3875](https://www.rfc-editor.org/rfc/rfc3875)
- [Simple HTTP webserver in C – bruinsslot.jp](https://bruinsslot.jp/post/simple-http-webserver-in-c/) — tutorial used to understand the general structure of an HTTP server in C
- [Codes de statut HTTP – MDN](https://developer.mozilla.org/fr/docs/Web/HTTP/Status)

### AI Usage

AI (Claude, Anthropic) was used punctually as a support tool, notably for :

- to reword and to re-structure notes taken during the research phase into a clear documentation
