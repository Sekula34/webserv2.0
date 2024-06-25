#include "ClientHeaderManager.hpp"
#include "ClientHeader.hpp"
#include <algorithm>
#include <cstddef>
#include <iostream>
#include <vector>

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
	std::cout << "Client Header with this fd already exist threfore it is not created" << std::endl;
	return;
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
			std::cout << "Client found in readHeaders" << std::endl;
			ClientHeader& toReturn(_readHeaders[i]);
			return toReturn;
		}
	}
	throw ClientHeaderNotFound();
}

/*void mark_client_done(int client_id) {
        auto it = std::find_if(active_clients.begin(),
			active_clients.end(), [client_id](const Client& client) {
            return client.id == client_id;
        });
		*/

ReadStatus ClientHeaderManager::readClientHeader(int& clientFD)
{
	createNewClientHeader(clientFD);
	ClientHeader& toRead = getClientHeader(clientFD);
	if(toRead.isFullyRead() == true)
	{
		return DONE;
	}
	ReadStatus status = toRead.readOnce();
	std::vector<ClientHeader>::iterator it = std::find_if(_unreadHeaders.begin(),
			_unreadHeaders.end(), FindClientByFd(clientFD));
	if(status == DONE)
	{
		_readHeaders.push_back(toRead);
		if(it != _unreadHeaders.end())
		{
			_unreadHeaders.erase(it);
		}
		else
		{
			std::cerr << "I didnt find clientHeader with that fd" << std::endl;
		}
		return DONE;
	}
	else if(status == ERROR)
	{
		if(it != _unreadHeaders.end())
		{
			_unreadHeaders.erase(it);
		}
		return ERROR;
	}
	return CONTINUE_READING;
}

const char* ClientHeaderManager::ClientHeaderNotFound::what() const throw()
{
	return ("NO SUCH CLIENT HEADER");
}
