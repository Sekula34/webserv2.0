#include "ClientHeaderManager.hpp"
#include "ClientHeader.hpp"
#include <cstddef>
#include <iostream>

ClientHeaderManager::ClientHeaderManager()
{

}

ClientHeaderManager::ClientHeaderManager(const ClientHeaderManager& source)
:_unreadHeaders(source._unreadHeaders), _readHeaders(source._unreadHeaders)
{

}

ClientHeaderManager& ClientHeaderManager::operator=(const ClientHeaderManager& source)
{
	_unreadHeaders = source._unreadHeaders;
	_readHeaders = source._readHeaders;
	return (*this);
}

ClientHeaderManager::~ClientHeaderManager()
{


}

void ClientHeaderManager::createNewClientHeader(int& clientFd)
{
	try 
	{
		getClientHeader(clientFd);
	}
	catch(ClientHeaderNotFound &e)
	{
		ClientHeader oneHeader(clientFd);
		_unreadHeaders.push_back(oneHeader);
		std::cout << "New header Created and addded to _unreadHeaders" << std::endl;
		return;
	}
	std::cout << "Client Header with this fd already exist" << std::endl;
}

ClientHeader& ClientHeaderManager::getClientHeader(int clientFd)
{
	for(size_t i = 0; i < _unreadHeaders.size(); i++)
	{
		if(clientFd == _unreadHeaders[i].getClientFd())
		{
			std::cout << "Client found in unreadHeaders" << std::endl;
			ClientHeader& toReturn(_unreadHeaders[i]);
			return toReturn;
		}
	}
	for(size_t i = 0; i < _readHeaders.size(); i++)
	{
		if(clientFd == _readHeaders[i].getClientFd())
		{
			std::cout << "Client found in unreadHeaders" << std::endl;
			ClientHeader& toReturn(_readHeaders[i]);
			return toReturn;
		}
	}
	throw ClientHeaderNotFound();
}

const char* ClientHeaderManager::ClientHeaderNotFound::what() const throw()
{
	return ("NO SUCH CLIENT HEADER");
}
