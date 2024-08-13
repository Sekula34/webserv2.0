#include "Autoindex.hpp"
#include "../Utils/Logger.hpp"

Autoindex::Autoindex(const std::string folderPath, int& statusCode)
:_folderPath(folderPath), _statusCode(statusCode)
{
	if(_statusCode != 0)
	{
		Logger::error("Trying to create Auto index but status code is not 0", true);
		return;
	}
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
