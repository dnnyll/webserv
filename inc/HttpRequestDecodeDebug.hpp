
#ifndef 	HTTPREQUESTDECODEDEBUG_HPP
# define	HTTPREQUESTDECODEDEBUG_HPP

#include	<iostream>
#include	<string>
#include	"../inc/HttpRequest.hpp"

template <typename T>
void	debugParse(const std::string& section,
				const std::string& label,
				const T& value)
{
	#ifdef DEBUG
	std::cout << "[" << section << "] "
			  << label << " = "
			  << value
			  << std::endl;
	#else
	(void)section;
	(void)label;
	(void)value;
	 #endif

}

#endif
