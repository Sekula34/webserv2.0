#include "Autoindex.hpp"
#include "Logger.hpp"
#include "FileUtils.hpp"
#include <cerrno>
#include <cstddef>
#include <cstdio>
#include <ostream>
#include <sstream>



Autoindex::Autoindex(const std::string folderPath, int& statusCode, const std::string linkBase)
:_folderPath(folderPath), _statusCode(statusCode), _linkBase(linkBase)
{
	if(_statusCode != 0)
	{
		Logger::error("Trying to create Auto index but status code is not 0", "");
		return;
	}
	if(FileUtils::isDirectoryValid(folderPath, statusCode) == false)
	{
		// perror("Checking directory failed");
		Logger::error("Directory is not valid", "");
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

const std::string& Autoindex::getAutoIndexHtml() const
{
	return (_autoIndexHtml);
}


void Autoindex::_createHtmlMenu()
{
	errno = 0;
	DIR* directory = opendir(_folderPath.c_str());
	if(directory == NULL)
	{
		_statusCode = 500;
		return;
	}
	_createDirectoryLinks(directory);
	closedir(directory);
	if(errno)
	{
		Logger::error("Reading dir", "");
		// perror("Reading dir");
		_statusCode = 500;
	}
	_generateHtmlPage("Auto index of folder: " +_folderPath);
}

void Autoindex::_createDirectoryLinks(DIR* directory)
{
	dirent* currentEntry = readdir(directory);
	size_t iteration = 0;
	while (currentEntry != NULL)
	{
		if(iteration > 1)
		{
			std::string fileName = currentEntry->d_name;
			if(currentEntry->d_type == DT_DIR)
				fileName += "/";
			_createAndStoreOneFileLink(fileName);
		}
		currentEntry = readdir(directory);
		iteration ++;
	}
}

void Autoindex::_createAndStoreOneFileLink(const std::string fileName)
{
	std::ostringstream oss;
	oss << "<a href=\"";
	oss << _linkBase;
	if(_linkBase.size() > 1 && _linkBase[_linkBase.size() - 1] != '/')
	{
		oss << "/";
	}
	//oss << "http://localhost:8080/autoindex/"; // full userUrl
	oss << fileName; //file link
	oss << "\">";
	oss << fileName;
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
