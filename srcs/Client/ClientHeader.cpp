#include "ClientHeader.hpp"
#include <cstddef>
#include <ostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>


ClientHeader::ClientHeader(int& clientFD)
:_clientFd(clientFD), _fullyRead(false)
{

}

ClientHeader::ClientHeader(const ClientHeader& source)
:_clientFd(source._clientFd), _message(source._message), _fullyRead(source._fullyRead)
{


}

ClientHeader&::ClientHeader::operator=(const ClientHeader& source)
{
	_message = source._message;
	return(*this);
}

ClientHeader::~ClientHeader()
{
	
}


ReadStatus ClientHeader::readOnce()
{
	if(_fullyRead == true)
	{
		std::cout << "NO need to read anymore" << std::endl;
		return DONE;
	}
	char buffer[BUFFER_SIZE];
	int retVal = recv(_clientFd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	std::cout << "Ret val is " << retVal << std::endl;
	if(retVal > 0)
	{
		_message.append(buffer, retVal);
		size_t lastCrlfPos = _message.find("\r\n\r\n");
		if(lastCrlfPos != std::string::npos)
		{
			_fullyRead = true;
			return DONE;
		}
		if(retVal < BUFFER_SIZE && lastCrlfPos == std::string::npos)
		{
			//didnt receive end of header but it is fully read
			std::cerr<<"//didnt receive end of header but it is fully read" << std::endl;
			return ERROR;
		} 
	}
	else if(retVal == -1)
	{
		//TODO server error of reading client request remove cerr
		close(_clientFd);
		// std::vector<int>::iterator it = std::find(_communicationFds.begin(),
		// 		_communicationFds.end(), communicationFd);
		// _communicationFds.erase(it);
		std::cerr<<"Read failed while trying to read client req" << std::endl;
		perror("read");
		return ERROR;
	}
	else if(retVal == 0 )
	{
		std::cout << "End of file " << std::endl;
		_fullyRead = true;
		return DONE;
	}
	return CONTINUE_READING;
}

const int& ClientHeader::getClientFd() const 
{
	return(_clientFd);
}

bool ClientHeader::isFullyRead() const
{
	return (_fullyRead);
}

std::ostream& operator<<(std::ostream& os, const ClientHeader& obj)
{
	os<< "Client message data" << std::endl;
	os<< "FD is :" << obj._clientFd << std::endl;
	os<< "IS fully read is " << obj._fullyRead << std::endl;
	os<< "Message is :[" << obj._message << "]";
	return os;
}
