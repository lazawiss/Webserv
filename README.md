*This project has been created as part of the 42 curriculum by ankim, dpaiva, lzannis.*

# Webserv

## Description

**Webserv** is an HTTP server written in **C++98**. The goal of this project is to build a web server modeled after **NGINX**, a widely used web server whose primary role is to receive HTTP requests from clients and respond to them. NGINX serves as the reference implementation for the general behavior of this project.

Webserv is built around the following features:

- reading a **configuration file** inspired by NGINX syntax;
- creating one or more **listening sockets** (one address:port pair per server);
- handling **multiple simultaneous clients** in a **non-blocking** manner,
using an event loop based on epoll();
- **parsing** incoming HTTP requests and responding with the **GET**, **POST**, and **DELETE** methods;
- serving **static files**, handling file **uploads**,
**directory listing** (autoindex), **redirects**,
and custom **error pages**;
- executing **CGI** scripts (Python and PHP) based on the requested file's extension;

## Instructions

### Prerequisites

- A **C++98**-compatible C++ compiler (`g++` or `clang++`)
- A **Linux** system (the event loop relies on `epoll`, which is Linux-specific)
- **Python 3** and/or **PHP-CGI** if you want to use the CGI features

### Compilation

```bash
git clone [url]
cd webserv
make
```

Compilation produces a `webserv` executable at the root of the project. The `Makefile` also provides the standard rules:

```bash
make clean   # removes object files
make fclean  # removes object files and the executable
make re      # fully recompiles the project
```

### Running

```bash
./webserv [configuration file]
```

A configuration file must be passed as an argument. Examples are available in `data/config/`.

### Configuration syntax

The configuration follows a syntax inspired by NGINX, with three levels: `global`, `server`, and `location`.

```nginx
server {
    listen       127.0.0.1:8080;
    server_name  tsuki;

    client_max_body_size 10M;

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

    location /cgi-bin/python {
        root            data/cgi-bin;
        methods         POST;
        cgi_extension   .py  /usr/bin/python3;
        index           index.py;
    }
}
```

**Available directives:**

| Directive | Context | Description |
|---|---|---|
| `listen` | `server` | IP address and listening port |
| `server_name` | `server` | Domain name (virtual hosting) |
| `client_max_body_size` | `global` / `server` / `location` | Maximum request body size |
| `root` | `global` / `server` / `location` | Root directory for served files |
| `index` | `global` / `server` / `location` | Default file to serve |
| `methods` | `location` | Allowed HTTP methods |
| `autoindex` | `global` / `server` / `location` | Enable directory listing |
| `return` | `location` | HTTP redirect |
| `error_page` | `global` / `server` / `location` | Custom error page |
| `cgi_extension` | `location` | Map file extension to CGI interpreter |

### Testing the server

Once running, the server listens on the addresses and ports defined in the configuration. It can be tested:

- with a web browser:

```bash
http://localhost:8080
````


- with `curl`:

```bash
curl -v http://127.0.0.1:8080/
```

- with a stress-testing tool (e.g. `siege`) to verify server stability under load.

## Resources

### Documentation and articles

- [Official NGINX documentation](https://nginx.org/en/docs/) — reference model for configuration syntax and general server behavior
- [RFC 9110 – HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110.html)
- [RFC 9112 – HTTP/1.1](https://www.rfc-editor.org/rfc/rfc9112.html)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) — classic reference for socket programming in C/C++
- [man epoll(7)](https://man7.org/linux/man-pages/man7/epoll.7.html)
- [The Common Gateway Interface (CGI) — RFC 3875](https://www.rfc-editor.org/rfc/rfc3875)
- [Simple HTTP webserver in C – bruinsslot.jp](https://bruinsslot.jp/post/simple-http-webserver-in-c/) — tutorial used to understand the general structure of an HTTP server in C
- [HTTP status codes – MDN](https://developer.mozilla.org/en-US/docs/Web/HTTP/Status)

### Use of AI

AI (Claude, Anthropic) was used occasionally as a supporting tool, in particular to:

- rephrase and structure working notes taken during the research phase into clear documentation.

<br>

---

<div align="center">

![alt text](image.png)

*You made it to the end!*

</div>
