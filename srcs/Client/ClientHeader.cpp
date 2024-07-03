#include "ClientHeader.hpp"
#include <cstddef>
#include <ostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <vector>
#include "../Parsing/ParsingUtils.hpp"
#include "../Utils/Logger.hpp"


ClientHeader::ClientHeader(int& clientFD)
:_clientFd(clientFD), _fullyRead(false)
{
	_host.name ="";
	_host.port = -1;
	_requestLineElements.requestMethod ="";
	_requestLineElements.requestTarget ="";
	_requestLineElements.protocolVersion = "";
}

ClientHeader::ClientHeader(const ClientHeader& source)
:_clientFd(source._clientFd), _message(source._message), _fullyRead(source._fullyRead),
_requestLine(source._requestLine), _requestLineElements(source._requestLineElements),
_host(source._host)
{


}

ClientHeader&::ClientHeader::operator=(const ClientHeader& source)
{
	_message = source._message;
	_fullyRead = source._fullyRead;
	_requestLine = source._requestLine;
	_requestLineElements = source._requestLineElements;
	_host = source._host;
	return(*this);
}

ClientHeader::~ClientHeader()
{
	
}


void ClientHeader::_setRequestLine(void) 
{
	std::string firstLine;
	size_t firstLineEnd =_message.find("\r\n");
	if(firstLineEnd == std::string::npos)
	{
		throw InvalidClientRequestException(400, "BAD REQUEST");
	}
	firstLine = _message.substr(0,firstLineEnd);
	_requestLine = firstLine;
}

void ClientHeader::_fillRequestStruct(void)
{
	std::vector<std::string> firstLineStrings = ParsingUtils::splitString(_requestLine, ' ');
	if(firstLineStrings.size() != 3)
	{
		throw InvalidClientRequestException(400, "BAD REQUEST");
	}
	_requestLineElements.requestMethod = firstLineStrings[0];
	_requestLineElements.requestTarget = firstLineStrings[1];
	_requestLineElements.protocolVersion = firstLineStrings[2];
}

void ClientHeader::_checkRequestStruct(void)
{
	const std::string validMethods[] = {"GET", "POST", "DELETE"};
	bool valid = ParsingUtils::isStringValid(_requestLineElements.requestMethod, validMethods, 3);
	if(valid == false)
	{
		//TODO:
		//Implement later to give this response to client since server
		//only implements those 3 methods
		throw InvalidClientRequestException(405, "METHOD NOT ALLOWED");
	}
	if(_requestLineElements.protocolVersion != "HTTP/1.1")
	{
		//TODO:
		//Implement later to give this response to client
		throw InvalidClientRequestException(505, "HTTP Version Not Supported");
	}
}

void ClientHeader::_setHost(void)
{
	std::string HostLine;
	size_t hosPos = _message.find("Host:");
	if(hosPos == std::string::npos)
	{
		throw InvalidClientRequestException(400, "Bad Request");
	}
	size_t endHos = _message.find("\r\n", hosPos);
	if(endHos == std::string::npos)
	{
		throw InvalidClientRequestException(400, "Bad Request");
	}
	HostLine = _message.substr(hosPos, endHos - hosPos);
	std::string plainHost = ParsingUtils::getHttpPlainValue(HostLine.substr(HostLine.find(':') + 1));
	std::vector<std::string> strings  = ParsingUtils::splitString(plainHost, ':');
	_host.name = strings[0];
	if(strings.size() == 1)
	{
		_host.port = 80;
	}
	else if(strings.size() == 2)
	{
		_host.port = ParsingUtils::stringToSizeT(strings[1]);
	}
	else 
	{
		throw InvalidClientRequestException(400, "Bad Request");
	}
}

bool ClientHeader::_isConnectionClosedByClient(void)
{
	char buffer[16];
	int bytes_read = recv(_clientFd, buffer, sizeof(buffer), MSG_PEEK);
	if(bytes_read == 0)
		return true;
	return false;
}

ReadStatus ClientHeader::readOnce()
{
	if(_fullyRead == true)
	{
		if(_isConnectionClosedByClient() == true)
			return CLIENT_CLOSE;
		//check if it is close by client
		std::cout << "NO need to read anymore" << std::endl;
		return DONE;
	}
	char buffer[BUFFER_SIZE];
	int retVal = recv(_clientFd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
	//std::cout << "Ret val is " << retVal << std::endl;
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
		//close(_clientFd);
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
		return CLIENT_CLOSE;
	}
	return CONTINUE_READING;
}


void ClientHeader::setCHVarivables()
{
	if(_fullyRead == false)
	{
		std::cout << "Cannot set client Header Variables (yet)" << std::endl;
		return;
	}
	_setRequestLine();
	_fillRequestStruct();
	_setHost();
	_checkRequestStruct();
	Logger::info("ALL header variables are setted and request is valid");std::cout<<std::endl;
	//std::cout << "ALL header variables are setted and request is valid" << std::endl;
}

const int& ClientHeader::getHostPort(void) const 
{
	return(_host.port);
}

const std::string& ClientHeader::getHostName(void) const 
{
	return(_host.name);
}

const std::string& ClientHeader::getFullMessage(void) const
{
	return (_message);
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
	if(obj._fullyRead == false)
	{
		os<<"CH dont have fully set all variables, valgrind read" << std::endl;
		return os;
	}
	os<< "Request method is :" << obj._requestLineElements.requestMethod << std::endl;
	os<< "Request target is :" << obj._requestLineElements.requestTarget << std::endl;
	os<< "Request protocol Version is :" << obj._requestLineElements.protocolVersion << std::endl;
	os<< "Host name is :" << obj._host.name << std::endl;
	os<< "Host port is :" << obj._host.port << std::endl;
	//os<< "Message is :[" << obj._message << "]";
	return os;
}


ClientHeader::
InvalidClientRequestException::InvalidClientRequestException(int errorCode, const std::string& errorMessage)
:_errorCode(errorCode), _errorMessage(errorMessage)
{

}
int ClientHeader::InvalidClientRequestException::getErrorCode() const 
{
	return _errorCode;
}

const char* ClientHeader::InvalidClientRequestException::what() const throw ()
{
	return (_errorMessage.c_str());
}
