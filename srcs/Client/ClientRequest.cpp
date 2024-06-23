#include "ClientRequest.hpp"
#include <cstddef>
#include <iostream>
#include <ostream>
#include <stdexcept>
#include <string>

ClientRequest::ClientRequest()
{
	std::cerr<<"Client Request should not be created like this" << std::endl;
	throw std::runtime_error("Invalid class");
}

ClientRequest::ClientRequest(std::string fullContent)
: _request(fullContent)
{
	_setRequestLine();
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
