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

void ClientHeaderManager::removeClient(int& clientFd)
{
	std::vector<ClientHeader>::iterator it = std::find_if(_readHeaders.begin(),
			_readHeaders.end(), FindClientByFd(clientFd));
	if(it != _readHeaders.end())
	{
		_readHeaders.erase(it);
	}
	it = std::find_if(_unreadHeaders.begin(),_unreadHeaders.end(), FindClientByFd(clientFd));
	if(it != _unreadHeaders.end())
	{
		_unreadHeaders.erase(it);
	}
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


/*
	TRY TO FIND communication Socket inside ClientHeader Unreadvector
	if it doesnt exist 
		//creatin new object put it in UnreadVector 
	if it exist 
		readOnce
			if read once == DONE 
				remove commSocket from read readyFD and removed it FROM UNreadVector
			if read once == error
				close connection?? removeid from client fd completely and from UNREAD vector
			if read once == continue
				//should read again //nothihg special?? 
*/
ReadStatus ClientHeaderManager::readClientHeader(int& clientFD)
{
	createNewClientHeader(clientFD);
	ClientHeader& toRead = getClientHeader(clientFD);
	if(toRead.isFullyRead() == true)
	{
		ReadStatus closed = toRead.readOnce();
		if(closed == CLIENT_CLOSE)
			return ERROR;
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
