#include "Response.hpp"
#include "ResponseHeader.hpp"
#include <ostream>

Response::Response(int& clientFd, const ResponseHeader& header)
:_clientFd(clientFd),_header(header), _code(-1), _body("")
{

}

Response::Response(const Response& source)
:_clientFd(source._clientFd), _header(source._header), _body(source._body)
{
	
}

Response&  Response::operator=(const Response& source)
{
	_body = source._body;
	return (*this);
}

Response::~Response()
{

}

std::ostream& operator<<(std::ostream& os, const Response& obj)
{
	os << "Response for client: " << obj._clientFd << std::endl;
	os << "Code of response is: " << obj._code << std::endl;
	os << "Response header is :" << std::endl;
	os << obj._header.turnResponseHeaderToString();
	os << "Body of response is : " << obj._body << std::endl;
	return os;
}
