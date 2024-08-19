#include "Autoindex.hpp"
#include "../Utils/Logger.hpp"
#include "../Utils/FileUtils.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <dirent.h>
#include <ostream>
#include <sstream>
#include <sys/types.h>


Autoindex::Autoindex(const std::string folderPath, int& statusCode)
:_folderPath(folderPath), _statusCode(statusCode)
{
	if(_statusCode != 0)
	{
		Logger::error("Trying to create Auto index but status code is not 0", true);
		return;
	}
	if(FileUtils::isDirectoryValid(folderPath, statusCode) == false)
	{
		perror("Checking directory failed");
		Logger::error("Directory is not valid", true);
		return;
	}
	_createHtmlMenu();
}

Autoindex::Autoindex(const Autoindex& source)
:_folderPath(source._folderPath), _statusCode(source._statusCode)
{

}

Autoindex& Autoindex::operator=(const Autoindex& source)
{
	(void) source;
	return (*this);
}

Autoindex::~Autoindex()
{

}

int& Autoindex::getStatusCode() const
{
	return (_statusCode);
}


void Autoindex::_createHtmlMenu()
{
	DIR* directory = opendir(_folderPath.c_str());
	if(directory == NULL)
	{
		_statusCode = 500;
		return;
	}
	dirent* currentEntry = readdir(directory);
	size_t iteration = 0;
	while (currentEntry != NULL)
	{
		if(iteration > 1)
			_createAndStoreOneFileLink(currentEntry->d_name);
		//std::cout << currentEntry->d_name << std::endl;
		currentEntry = readdir(directory);
		iteration ++;
	}
	closedir(directory);
	if(errno)
	{
		perror("Reading dir");
		_statusCode = 500;
	}
	_generateHtmlPage("Title");
}

void Autoindex::_createAndStoreOneFileLink(const std::string fileEntry)
{
	std::ostringstream oss;
	oss << "<a href=\"";
	oss << fileEntry;
	oss << "\">";
	oss << fileEntry;
	oss << "</a>";

	const std::string stringLink = oss.str();
	_fileLinks.push_back(stringLink);
}

void Autoindex::_generateHtmlPage(const std::string pageTitle)
{
	std::ostringstream oss;
	oss << "<html>";
	oss << "<head><title>";
	oss << pageTitle;
	oss << "</title></head><body><h1>";
	oss << pageTitle; 
	oss << "</h1><hr><pre>\n<a href=\"../\">../</a>\n";

	for(size_t i = 0; i < _fileLinks.size(); i ++)
	{
		oss << _fileLinks[i];
		oss << "\n";
	}
	oss << "</pre><hr></body></html>";
	_autoIndexHtml = oss.str();
}
