#ifndef FILEUTILS_HPP
# define FILEUTILS_HPP
#include <string>

class FileUtils
{
	public :
		static bool isPathValid(const std::string relativeFilePath);
		/**
		 * @brief put file in string 
		 * 
		 * @param relativeFilePath to the file, (Relative to executable webserv)
		 * @param string in which file will be putted
		 * @return true file is regular and it is succesfully in string
		 * @return false either file is not regular, not permission, or something failed, generate 500 http code in that case
		 */
		static bool putFileInString(const std::string relativeFilePath, std::string& string);
};

#endif
