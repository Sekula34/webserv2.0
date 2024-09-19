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
_responseBody(client, _server)
{
	_responseHeader = new ResponseHeader(_responseBody.getHttpStatusCode(), _responseBody.getResponse().size()); // MR_NOTE: Try/catch block?
}

//maybe broken
Response::Response(const Response& source)
:_client(source._client),
_responseBody(source._responseBody)
{
	if(source._responseHeader == NULL)
		_responseHeader = NULL;
	else  
		_responseHeader = new ResponseHeader(*source._responseHeader); // MR_NOTE: Try/catch block?
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
		_responseHeader = new ResponseHeader(httpCode, _responseBody.getResponse().size()); // MR_NOTE: Try/catch block?
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

// MR_NOTE: This is not on a loop. So if we can only write partial msg, then how it's handled?
bool Response::sendResponse()
{
	std::string response = _createResponseString();
	Logger::info("String Response created: ", true);
 	int writeValue;
	writeValue = write(_client.getFd(), response.c_str(), response.size());
	if(writeValue == -1)
		return false;
	return true;
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
