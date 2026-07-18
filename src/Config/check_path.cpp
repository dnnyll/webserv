#include <string>
#include <exception>
#include <sys/stat.h>
#include <unistd.h>
#include "../../inc/check_path.hpp"

#include <iostream> //pour debbug

/**
 * @brief Function to check the file configuration extension.
 * The good format is "something".conf. The path can be relative
 * or absolute.
 * @param path The path to the configuration file
 * @exception Throw WrongFormatException if there is an error in the path
 */
static void checkExtension(const std::string& path)
{
	size_t		pos;
	std::string	str;

	pos = path.rfind(".");
	str = path.substr(pos, 5);
	if (str.compare(".conf") != 0)
		throw WrongFormatException();
}

static void checkExist(const std::string& path)
{
	struct stat buffer;

	if (stat(path.c_str(), &buffer) != 0)
		throw ExistException();
}

static void checkIsRegular(const std::string& path)
{
	struct stat	buffer;

	stat(path.c_str(), &buffer);
	if (!S_ISREG(buffer.st_mode))
		throw RegularException();
}

static void checkReadable(const std::string& path)
{
	if (access(path.c_str(), R_OK) != 0)
		throw PermissionException();
}

static void checkNotEmpty(const std::string& path)
{
	struct stat buffer;

	stat(path.c_str(), &buffer);
	if(buffer.st_size == 0)
		throw EmptyFileException();
}

void check(const std::string& path)
{
	checkExtension(path);
	checkExist(path);
	checkIsRegular(path);
	checkReadable(path);
	checkNotEmpty(path);
}

const char* WrongFormatException::what() const throw()
{
	return ("Wrong path format!");
}

const char* ExistException::what() const throw()
{
	return ("File doesn't exist!");
}

const char* RegularException::what() const throw()
{
	return ("This file is not a regular file!");
}

const char* PermissionException::what() const throw()
{
	return ("Impossible to read the file!");
}

const char* EmptyFileException::what() const throw()
{
	return ("The configuration file is empty!");
}
