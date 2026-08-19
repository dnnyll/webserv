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

## 🖥️ Usage

### Start the server

### Testing with commands

## 📁 Project structure

## 📄 Resources

### References

### IA Usage