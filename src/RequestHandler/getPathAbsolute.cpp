#include "RequestHandler.hpp"

std::string	RequestHandler::getPathAbsolute(std::string uri, std::string root,
						 std::string locationPath)
{
	if (uri.find("..") != std::string::npos)
		return ("");
	return (uri.replace(0, locationPath.size(), root, 0, root.size()));
}
//basic_string& replace( size_type pos, size_type count,
//                       const basic_string& str,
//                       size_type pos2, size_type count2 );
//
//
//pos 	- 	start of the substring that is going to be replaced
//count 	- 	length of the substring that is going to be replaced
//first, last 	- 	range of characters that is going to be replaced
//str 	- 	string to use for replacement
//pos2 	- 	start of the substring to replace with
//count2 	- 	number of characters to replace with
//cstr 	- 	pointer to the character string to use for replacement
//ch 	- 	character value to use for replacement
//first2, last2 	- 	range of characters to use for replacement
//ilist 	- 	initializer list with the characters to use for replacement
//t 	- 	object (convertible to std::basic_string_view) with the characters to use for replacement
//
