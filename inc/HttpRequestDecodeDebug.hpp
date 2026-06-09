#include	"../inc/HttpRequest.hpp"

//	debug for HttpResquestDecode -> void HttpRequest::decodeRequestLine()

template <typename T>
void debugRequestLine(const std::string& label, const T& value)
{
	std::cout << "[decodeRequestLine] "
			  << label
			  << " = "
			  << value
			  << std::endl;
}