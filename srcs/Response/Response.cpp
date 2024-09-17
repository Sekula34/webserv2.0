#include "Response.hpp"
#include "ResponseBody.hpp"
#include "ResponseHeader.hpp"
#include <ostream>
#include <iostream>
#include "../Utils/Logger.hpp"
#include <cstring>
#include <sstream>
#include <unistd.h>
#include "../Client/Client.hpp"

Response::Response(const Client& client, const ServerSettings* server)
:_client(client),
_server(server),
_responseHeader(NULL),
_responseBody(client, _server),
_bytesSent(0)
{
	_responseHeader = new ResponseHeader(_responseBody.getHttpStatusCode(), _responseBody.getResponse().size());
}

//maybe broken
Response::Response(const Response& source)
:_client(source._client),
_responseBody(source._responseBody),
_bytesSent(source._bytesSent)
{
	if(source._responseHeader == NULL)
		_responseHeader = NULL;
	else  
		_responseHeader = new ResponseHeader(*source._responseHeader);
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

std::string Response::getResponseString(void)
{
	std::string res = _createResponseString();
	return res;
}
size_t	Response::getBytesSent()
{
	return (_bytesSent);
}


std::string Response::_createResponseString(void) 
{
	std::ostringstream oss;
	int httpCode = _responseBody.getHttpStatusCode();
	if(httpCode == 0)
	{
		Logger::error("Response body is not Created", true);
	}
	//created header
	if(_responseHeader == NULL)
	{
		_responseHeader = new ResponseHeader(httpCode, _responseBody.getResponse().size());
	}
	oss << _responseHeader->turnResponseHeaderToString();
	oss << "\r\n";
	oss << _responseBody.getResponse();


	// std::cout << "oss full response" << std::endl;
	// std::cout << oss.str() << std::endl;
	return oss.str();
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
	(void) http_response;
	// write(_clientHeader.getClientFd() , http_response , strlen(http_response));
	// Logger::info("Response is sent to client: "); std::cout <<_clientHeader.getClientFd() << std::endl;
}

bool Response::sendResponse()
{
 	int writeValue = 0;
	std::string response = _createResponseString();
	Logger::info("String Response created: ", true);

	// SEND
	writeValue = send(_client.getFd(),
		response.c_str() + _bytesSent, response.size() - _bytesSent, MSG_DONTWAIT | MSG_NOSIGNAL);
	_bytesSent += writeValue;

	// return out of function if full Message was not sent because
	// message bigger than socket buffer
	if (_bytesSent < response.size() && writeValue > 0)
		return (false);

	// if unable to send full message, log error and set error Code
	if (writeValue < 0 || (writeValue == 0 && _bytesSent < response.size()))
		Logger::error("failed to send Response to Client", true);
	return (true);
}

std::ostream& operator<<(std::ostream& os, const Response& obj)
{
	//os << "Response for client: " << obj._clientHeader.getClientFd() << std::endl;
	os << "Response header is :" << std::endl;
	if(obj._responseHeader != NULL)
		os << obj._responseHeader->turnResponseHeaderToString();
	else   
		os<< "Header is NULL" << std::endl;
	return os;
}
