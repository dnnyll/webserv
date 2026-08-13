#ifndef		HTTPMESSAGE_HPP
#define		HTTPMESSAGE_HPP

#include	<string>
#include	<map>
#include	<iostream>

class	HttpMessage
{
	public:
		std::string	version;
		std::map<std::string, std::string> headers;
		std::string	body;

	public:
		virtual	~HttpMessage();

	protected:
		HttpMessage();
};

#endif
