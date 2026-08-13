#ifndef		HTTPREQUEST_HPP
# define	HTTPREQUEST_HPP

#include	<string>
#include	<map>
#include	"../inc/HttpMessage.hpp"

enum	ParseState
{
	REQUEST_LINE,
	HEADERS,
	BODY,
	CHUNKED,
	COMPLETE,
	ERROR_STATE
};

enum	ErrorReason
{
	NO_ERROR,
	BODY_TOO_LARGE,
	METHOD_NOT_ALLOWED,
	MALFORMED_REQUEST
};

class	HttpRequest :public HttpMessage
{
	public:
		std::string		method;
		std::string		uri;
		size_t			contentLength;
		bool			isChunked;
		size_t			_chunkSize;

	private:
		ParseState		_state;
		ErrorReason		_errorReason;
		std::string		_buffer;
		size_t			_bodyBytesRead;
		size_t			_maxBodySize;

	public:
		HttpRequest();
		void		setMaxBodySize(size_t size);
		bool		getData(const std::string& chunk);
		bool		decode();
		void		reset();
		bool		isComplete() const;
		bool		hasError() const;
		ErrorReason	getErrorReason() const;
		
	private:
		//	DecodeRequestLine
		bool		extractRequestLine(std::string &line);
		bool		splitRequestLine(const std::string &line);
		bool		validateRequestLine();
		void		decodeRequestLine();

		//	DecodeHeaders
		bool		extractHeaderLine(std::string &line, size_t &pos);
		bool		splitHeaderLine(const std::string &line);
		void		resolveBodyState();
		void		decodeHeaders();

		//	DecodeBody
		void		decodeBody();

		//	DecodeChunked
		bool		parseChunkSize();
		void		decodeChunked();
};

#endif
