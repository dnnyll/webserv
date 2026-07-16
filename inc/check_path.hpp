#ifndef CHECK_PATH_HPP
# define CHECK_PATH_HPP

# include <string>
# include <exception>

void check(const std::string& path);

class WrongFormatException : public std::exception
{
	public:
		virtual const char* what() const throw();
};
class ExistException : public std::exception
{
	public:
		virtual const char* what() const throw();
};
class RegularException : public std::exception
{
	public:
		virtual const char* what() const throw();
};
class EmptyFileException : public std::exception
{
	public:
		virtual const char* what() const throw();
};
class PermissionException : public std::exception
{
	public:
		virtual const char* what() const throw();
};

#endif