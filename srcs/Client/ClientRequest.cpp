#include "ClientRequest.hpp"
#include <iostream>
#include <ostream>
#include <stdexcept>

ClientRequest::ClientRequest()
{
	std::cerr<<"Client Request should not be created like this" << std::endl;
	throw std::runtime_error("Invalid class");
}

ClientRequest::ClientRequest(std::string fullContent)
: _request(fullContent)
{

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

std::ostream& operator<<(std::ostream &os, const ClientRequest& object)
{
	os<< "Client request is :" << std::endl;
	os<< object._request << std::endl;
	os<<"----END OF REQUEST----";
	return os;
}
