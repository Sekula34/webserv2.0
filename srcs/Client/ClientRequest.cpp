#include "ClientRequest.hpp"
#include <cstddef>
#include <iostream>
#include <ostream>
#include <string>
#include "../Parsing/ParsingUtils.hpp"

ClientRequest::ClientRequest()
{
	std::cerr<<"Client Request should not be created like this" << std::endl;
	throw InvalidClientRequestException(400, "BAD REQUEST");
}

ClientRequest::ClientRequest(std::string fullContent)
: _request(fullContent)
{
	_setRequestLine();
	_fillRequestStruct();
	_checkRequestStruct();
	_setHost();
	std::cout << "Request line is [" << _requestLine << "]" << std::endl;
}

ClientRequest::ClientRequest(const ClientRequest& source)
:_request(source._request)
{
}

ClientRequest& ClientRequest::operator=(const ClientRequest& source)
{
	_request = source._request;
	return(*this);
}

ClientRequest::~ClientRequest()
{

}

void ClientRequest::_setRequestLine(void) 
{
	std::string firstLine;
	size_t firstLineEnd =_request.find("\r\n");
	if(firstLineEnd == std::string::npos)
	{
		throw InvalidClientRequestException(400, "BAD REQUEST");
	}
	firstLine = _request.substr(0,firstLineEnd);
	_requestLine = firstLine;
}

void ClientRequest::_fillRequestStruct(void)
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

void ClientRequest::_checkRequestStruct(void)
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

void ClientRequest::_setHost(void)
{
	std::string HostLine;
	size_t hosPos = _request.find("Host:");
	if(hosPos == std::string::npos)
	{
		throw InvalidClientRequestException(400, "Bad Request");
	}
	size_t endHos = _request.find("\r\n", hosPos);
	if(endHos == std::string::npos)
	{
		throw InvalidClientRequestException(400, "Bad Request");
	}
	HostLine = _request.substr(hosPos, endHos - hosPos);
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

std::ostream& operator<<(std::ostream &os, const ClientRequest& object)
{
	os<< "Client request is :" << std::endl;
	os<< object._request << std::endl;
	os<<"----END OF REQUEST----";
	return os;
}

ClientRequest::
InvalidClientRequestException::InvalidClientRequestException(int errorCode, const std::string& errorMessage)
:_errorCode(errorCode), _errorMessage(errorMessage)
{

}
int ClientRequest::InvalidClientRequestException::getErrorCode() const 
{
	return _errorCode;
}

const char* ClientRequest::InvalidClientRequestException::what() const throw ()
{
	return (_errorMessage.c_str());
}
