#include "ClientRequest.hpp"
#include <cstddef>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>
#include "../Parsing/ParsingUtils.hpp"

ClientRequest::ClientRequest()
{
	std::cerr<<"Client Request should not be created like this" << std::endl;
	throw std::runtime_error("Invalid class");
}

ClientRequest::ClientRequest(std::string fullContent)
: _request(fullContent)
{
	_setRequestLine();
	_fillRequestStruct();
	_checkRequestStruct();
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
		throw InvalidClientRequestException();
	}
	firstLine = _request.substr(0,firstLineEnd);
	_requestLine = firstLine;
}

void ClientRequest::_fillRequestStruct(void)
{
	std::vector<std::string> firstLineStrings = ParsingUtils::splitString(_requestLine, ' ');
	if(firstLineStrings.size() != 3)
	{
		throw InvalidClientRequestException();
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
		throw std::runtime_error("405 Method Not Allowed");
	}
	if(_requestLineElements.protocolVersion != "HTTP/1.1")
	{
		//TODO:
		//Implement later to give this response to client
		throw std::runtime_error("A server can send a 505(HTTP Version Not Supported");
	}
}

std::ostream& operator<<(std::ostream &os, const ClientRequest& object)
{
	os<< "Client request is :" << std::endl;
	os<< object._request << std::endl;
	os<<"----END OF REQUEST----";
	return os;
}

const char* ClientRequest::InvalidClientRequestException::what() const throw ()
{
	return ("Invalid Client Request Recieved");
}
