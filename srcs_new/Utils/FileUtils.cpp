#include "FileUtils.hpp"
#include <asm-generic/errno.h>
#include <cerrno>
#include <cstddef>
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


const std::string FileUtils::getFileExtension(const std::string &fullFileName)
{
	size_t extensionPos = fullFileName.rfind('.');
	if(extensionPos == std::string::npos)
		return ("");
	const std::string& fileExtension = fullFileName.substr(extensionPos);
	Logger::warning("file extension is", fileExtension);
	return fileExtension;
}
const std::string FileUtils::getFileName(const std::string& fullFileName)
{
	const std::string& fileExtension = getFileExtension(fullFileName);
	if(fileExtension == "")
		return fullFileName;
	const std::string& fileName = fullFileName.substr(0, fullFileName.size() - fileExtension.size());
	return fileName;
}

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
	Logger::info("Trying to open ", relativeFilePath);
	if(file.good() == false)
	{
		Logger::error("Opening file failed. Check if file exist (maybe typo) and if you have reading permmision.", "");
		return false;
	}
	struct stat fileStat;
	if(stat(relativeFilePath.c_str(), &fileStat) != 0)
	{
		throw std::runtime_error("Stat function failed");
	}
	if(S_ISREG(fileStat.st_mode) == 0)
	{
		Logger::error("File is not regular at location: ", relativeFilePath);
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

// Method to check if a given filename or foldername is valid:
// - If it does not contain path travelsal sequences like '../' or '~/'
// - If the name is not empty, and it's not '.', '..' or '~'
bool	FileUtils::isValidName(const std::string& fileName)
{
	if (fileName.empty() == true)
	{
		Logger::warning("file folder name is empty!","");
		return (false);
	}
	size_t len = fileName.length(); // This var is used to avoid out-of-bounds access. 
	if (len == 1 && (fileName[0] == '.' || fileName[0] == '~'))
		return (false);
	if (len == 2 && (fileName[0] == '.' && fileName[1] == '.'))
		return (false);
	if (fileName.find("~/") == 0 || fileName.find("../") == 0)
		return (false);
	size_t pos = fileName.find("/~");
	if (pos != std::string::npos
		 && (len >= pos + 2) && (fileName[pos + 2] == '/' || fileName[pos + 2] == '\0'))
		 return (false);
	pos = fileName.find("/..");
	if (pos != std::string::npos
		 && (len >= pos + 3) && (fileName[pos + 3] == '/' || fileName[pos + 3] == '\0'))
		 return (false);
	return (true);
}

int FileUtils::isPathFileOrFolder(const std::string &serverFilePath, int& httpStatusCode)
{
	struct stat statBuf;
	errno = 0;
	int value = stat(serverFilePath.c_str(), &statBuf);
	if(value != 0)
	{
		_setFileOrFolderStatusCode(errno, httpStatusCode);
		return -1;
	}
	if(statBuf.st_mode & S_IFREG)
	{
		Logger::info("It is file", "");
		return 1;
	}
	else if(statBuf.st_mode & S_IFDIR)
	{
		Logger::info("It is directory", "");
		return 2;
	}
	else
	{
		Logger::info("Only God knows", "");
		return 0;
	}
}

void FileUtils::_setFileOrFolderStatusCode(int errnoNum, int& httpStatusCode)
{
	// perror("File or Folder fail: "); //FIXME: delete for eval
	Logger::error("File or Folder fail", "");
	if(errnoNum == EACCES)
		httpStatusCode = 403;
	else if(errnoNum == EBADF || errnoNum == ENONET || errnoNum == 2)
		httpStatusCode = 404;
	else
		httpStatusCode = 500;
}
