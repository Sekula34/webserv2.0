#include "Response.hpp"
#include "ResponseBody.hpp"
#include "ResponseHeader.hpp"
#include <ostream>
#include <iostream>
#include "../Utils/Logger.hpp"
#include <cstring>
#include <unistd.h>

Response::Response(ClientHeader& clientHeader, const ServerSettings& server)
:_clientHeader(clientHeader), _server(server) ,_responseHeader(NULL)
,_responseBody(_clientHeader, _server)
{

}

Response::Response(const Response& source)
:_clientHeader(source._clientHeader), _server(source._server),
 _responseHeader(source._responseHeader), _responseBody(source._responseBody)
{
	
}

Response&  Response::operator=(const Response& source)
{
	(void) source;
	return (*this);
}

Response::~Response()
{
	delete _responseHeader;
}

//for testing only
void Response::sendSimpleResponse()const 
{
	//std::cout << "INFO: i am sending response" << std::endl;
	const char *http_response = 
	"HTTP/1.1 200 OK\r\n"
	"Content-Type: text/html\r\n"
	"Content-Length: 48\r\n"
	"Connection: close\r\n"
	"\r\n"
	"<html><body><h1>Hello, World!</h1></body></html>";
	write(_clientHeader.getClientFd() , http_response , strlen(http_response));
	Logger::info("Response is sent to client: "); std::cout <<_clientHeader.getClientFd() << std::endl;
}

std::ostream& operator<<(std::ostream& os, const Response& obj)
{
	os << "Response for client: " << obj._clientHeader.getClientFd() << std::endl;
	os << "Response header is :" << std::endl;
	if(obj._responseHeader != NULL)
		os << obj._responseHeader->turnResponseHeaderToString();
	else   
		os<< "Header is NULL" << std::endl;
	return os;
}
