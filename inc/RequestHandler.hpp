#ifndef		REQUESTHANDLER_HPP
# define	REQUESTHANDLER_HPP

#include "HttpRequest.hpp"
#include "HttpResponse.hpp"
#include "Config.hpp"

class	RequestHandler
{
	private:
		const HttpRequest	&_request;
		const ServerBlock	&_config;
		HttpResponse		_response;

		const Location		*_location;
		std::string			_pathAbsolute;
		//methode priver a voir si toute necessaire
		int					setLocation();
		//void	checkPermision();
		//void	handleGet();
		//void	handlePost();
		//void	handleDelete();
		//bool executeCGI();
		//void	generateErrorPage(int statusCode);

	public:
		RequestHandler(const HttpRequest &request, const ServerBlock &config);

		HttpResponse	process();
};

//_pathAbsolute = getpathAbsolute(_request.uri, actLocation.root, actLocation.path);
//static std::string	getPathAbsolute(std::string uri, std::string root,
//						std::string path)
//{
//	return (path.replace(0, path.size(), root, 0, root.size());
//}
////basic_string& replace( size_type pos, size_type count,
////                       const basic_string& str,
////                       size_type pos2, size_type count2 );
////
////
////pos 	- 	start of the substring that is going to be replaced
////count 	- 	length of the substring that is going to be replaced
////first, last 	- 	range of characters that is going to be replaced
////str 	- 	string to use for replacement
////pos2 	- 	start of the substring to replace with
////count2 	- 	number of characters to replace with
////cstr 	- 	pointer to the character string to use for replacement
////ch 	- 	character value to use for replacement
////first2, last2 	- 	range of characters to use for replacement
////ilist 	- 	initializer list with the characters to use for replacement
////t 	- 	object (convertible to std::basic_string_view) with the characters to use for replacement
////

#endif
