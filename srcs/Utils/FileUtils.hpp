#ifndef FILEUTILS_HPP
# define FILEUTILS_HPP
#include <string>

class FileUtils
{
	public :
		static bool isPathValid(const std::string relativeFilePath);
		static bool putFileInString(const std::string relativeFilePath, std::string& string);
};

#endif
