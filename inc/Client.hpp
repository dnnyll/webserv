#ifndef		CLIENT_HPP
# define	CLIENT_HPP

#include	<string>
#include	"HttpRequest.hpp"
#include	"HttpResponse.hpp"

enum	State
{
	READING,
	PROCESSING,
	WRITING,
	CLOSE
};

class	Client
{
	public:
		Client();

	private:
		int				fd;
		State			state;
		std::string		raw_request;
		HttpRequest		request;
		HttpResponse	response;
		size_t			bytes_send;
		time_t			last_activity;
		pid_t			cgi_pid;
		bool			cgi_running;
		bool			header_complete;
		bool			body_complete;
		size_t			content_length;
};

#endif

//à supprimer - not use