#include "FileUtils.hpp"
#include <asm-generic/errno.h>
#include <cerrno>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include "Logger.hpp"
#include <sstream>
#include <string>
#include <sys/stat.h> 
#include <sys/types.h>
#include <dirent.h>


std::string FileUtils::_configFilePath;

void FileUtils::setConfigFilePath(const std::string &configFilePath)
{
	FileUtils::_configFilePath = configFilePath;
}

const std::string& FileUtils::getConfigFilePath(void)
{
	return _configFilePath;
}

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
	struct stat fileStat; // MR_DOUBT: Is it ok to check like this?
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
	stringFile.erase(); // MR_DOUBT: Why is this erased?
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

// MR_NOTE: errno should be set to zero at the start of this function.
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



int FileUtils::isPathFileOrFolder(const std::string &serverFilePath, int& httpStatusCode)
{
	struct stat statBuf; // MR_DOUBT: Is it ok to check like this?
	errno = 0;
	int value = stat(serverFilePath.c_str(), &statBuf);
	if(value != 0)
	{
		_setFileOrFolderStatusCode(errno, httpStatusCode);
		return -1;
	}
	if(statBuf.st_mode & S_IFREG)
	{
		Logger::info("It is file", true);
		return 1;
	}
	else if(statBuf.st_mode & S_IFDIR)
	{
		Logger::info("It is directory", true);
		return 2;
	}
	else
	{
		Logger::info("Only God knows", true);
		return 0;
	}
}

void FileUtils::_setFileOrFolderStatusCode(int errnoNum, int& httpStatusCode)
{
	perror("File or Folder fail: ");
	if(errnoNum == EACCES)
		httpStatusCode = 403;
	else if(errnoNum == EBADF || errnoNum == ENONET || errnoNum == 2)
		httpStatusCode = 404;
	else
		httpStatusCode = 500;
}
