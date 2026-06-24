#ifndef EVENT_LOOP_HPP
# define EVENT_LOOP_HPP

#include	<poll.h>
#include	<iostream>
#include	<string.h>
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<unistd.h>
#include 	<arpa/inet.h>
#include 	<netinet/in.h>
#include	<vector>
#include	<map>
#include	<fcntl.h>
 
#define BUFFER_SIZE 50

struct Client{
	int	fd;
	std::string receive;
	std::string send;
};

int	event_loop(void);

#endif