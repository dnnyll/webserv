_This project has been created as part of the 42 curriculum by daniefe2, jsurian et alfavre._

# WebServ - Create a HTTP server and test it with an actual browser

## Description

### 🎯 About

**WebServ** is a lightweight HTTP/1.1 server implemented from scratch in C++98, developed as part of the 42 school curriculum.

The goal of this project is to understand how a web server works at a low level by building one capable of handling real requests from an actual web browse, without relying on any external library for the HTTP or networking layer.

The server parses an NGINX-style configuration file to define one or multiple virtual hosts, handles multiple client connections concurrently through non-blocking I/O (using [poll]), and supports the [GET, POST, DELETE] methods.

It can serve static files, execute CGI scripts, handle file uploads and return custom error pages, all while staying compliant with a relevant subset of the HTTP/1.1 protocol.


## 🔧 Instructions

### Prerequisites

 - Compiler: g++ with C++98 support
 - OS: build and test on Linux
 - Build tool: make

### Build steps

```
# Clone the repository
git clone https://github.com/dnnyll/webserv.git
cd webserv

# Compile the project
make

-> an executable webserv will be created
```

### Make commands

```
# Build the project
make

# Remove object files
make clean

# Remove object files and executable
make fclean

# Rebuild frome scratch
make re

# Build the project with the flag DEBUG=1
make debug
```

## ⚙️ Configuration

### Configuration file format

The server use a NGINX-style format.

```
server {
	listen 8080;
	server_name webinterface;
	client_max_body_size 10000000;
	root www;

	# custom error pages served from www/errors
	error_page 400 /errors/400.html;

	# serves www/index/index.html at "/"
	location / {
		root                    www/index;
		index                   index.html;
		autoindex               off;
		methods                 GET;
	}
}
```

### Configuration directives

**Server block**

| Directive             | Description                          | Example                  |
|------------------------|---------------------------------------|---------------------------|
| `listen`               | Port the server listens on            | `listen 8080;`             |
| `server_name`          | Virtual host name                     | `server_name webinterface;`|
| `client_max_body_size` | Max size of a request body (bytes)    | `client_max_body_size 10000000;` |
| `root`                 | Default document root for the server  | `root www;`                |
| `error_page`           | Custom error page for a status code   | `error_page 400 /errors/400.html;` |

**Location block**

| Directive   | Description                              | Example              |
|-------------|--------------------------------------------|------------------------|
| `root`      | Document root for this location            | `root www/index;`      |
| `index`     | Default file served for a directory request | `index index.html;`   |
| `autoindex` | Enable/disable directory listing            | `autoindex off;`       |
| `methods`   | HTTP methods allowed on this location       | `methods GET;`         |

## 🖥️ Usage

### Start the server

```bash
# Use the default configuration file
./webserv config_files/webinterface.conf

# Use a custom configuration file
./webserv config_files/<custom.conf>
```

### Testing with commands

```bash
# GET request
curl http://localhost:8080/

# POST request
curl -v -X POST -d "hello from curl" http://localhost:8080/upload/<filename>

# DELETE request
curl -v -X DELETE http://localhost:8080/www/upload/<filename>
```

## 📄 Resources

### References

- https://medium.com/from-the-scratch/http-server-what-do-you-need-to-know-to-build-a-simple-http-server-from-scratch-d1ef8945e4fa
- https://fr.wikipedia.org/wiki/Hypertext_Transfer_Protocol
- https://hackmd.io/@laian/SJZHcOsmT
- https://nginx.org/en/docs/
- https://datatracker.ietf.org/doc/html/rfc7230
- https://datatracker.ietf.org/doc/html/rfc7231
- https://datatracker.ietf.org/doc/html/rfc3875
- https://developer.mozilla.org/en-US/docs/Web/HTTP/Guides/Overview
- https://www.dre.vanderbilt.edu/~schmidt/PDF/reactor-siemens.pdf

### IA Usage

AI was used during this project for the following tasks:

- Debugging Git workflow issues
- Designing the manual test suite page
- ReadMe structuring
- Help to solve problems during the development part

AI was not used to write the core C++ server logic.