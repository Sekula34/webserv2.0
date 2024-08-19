#include "FileUtils.hpp"
#include <asm-generic/errno.h>
#include <cerrno>
#include <fstream>
#include <iostream>
#include "Logger.hpp"
#include <sstream>
#include <string>
#include <sys/stat.h> 
#include <sys/types.h>
#include <dirent.h>


bool FileUtils::isPathValid(const std::string relativeFilePath)
{
	if(relativeFilePath.empty() == true)
	{
		Logger::error("Provided path to file is empty.", true);
		return false;
	}
	std::ifstream file(relativeFilePath.c_str());
	Logger::info("Trying to open "); std::cout << relativeFilePath.c_str() << std::endl;
	if(file.good() == false)
	{
		Logger::error("Opening file failed. Check if file exist (maybe typo) and if you have reading permmision.", true);
		return false;
	}
	struct stat fileStat;
	if(stat(relativeFilePath.c_str(), &fileStat) != 0)
	{
		throw std::runtime_error("Stat function failed");
	}
	if(S_ISREG(fileStat.st_mode) == 0)
	{
		Logger::error("File is not regular at location: "); std::cout << relativeFilePath;
		return false;
	}
	Logger::info("File is regular file that can be opened", true);
	return true;
}

bool FileUtils::putFileInString(const std::string filePath, std::string &stringFile)
{
	const std::string relativePath = "./" + filePath;
	stringFile.erase();
	if(isPathValid(relativePath) == false)
	{
		return false;
	}
	std::ifstream file(relativePath.c_str());
	if(!file.is_open())
	{
		Logger::error("Cannot open file, this should not happen", true);
		return false;
	}
	std::stringstream ss;
	ss << file.rdbuf();
	if(file.fail())
	{
		Logger::error("String stream failed", true);
		return false;
	}
	file.close();
	stringFile = ss.str();
	return true;
}

bool FileUtils::isDirectoryValid(const std::string relativeDirPath, int& httpStatusCode)
{
	DIR* directory = opendir(relativeDirPath.c_str());
	if(directory == NULL)
	{
		_setDirFailStatusCode(errno, httpStatusCode);
		return false;
	}
	closedir(directory);
	return true;
}

void FileUtils::_setDirFailStatusCode(int errnoNum, int& httpStatusCode)
{
	if(errnoNum == EACCES || errnoNum == EBADF)
	{
		httpStatusCode = 403;
		return;
	}
	if(errnoNum == ENONET || errnoNum == ENOTDIR)
	{
		httpStatusCode = 404;
		return;
	}
	httpStatusCode = 500;
}
