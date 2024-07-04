#include "ClientResponse.hpp"
#include "ClientHeader.hpp"
#include <cstring>
#include <ostream>
#include <iostream>
#include <unistd.h>
#include "../Utils/Logger.hpp"


ClientResponse::ClientResponse(ClientHeader& header,
	const ServerSettings& server)
:_clientHeader(header), _server(server)
{
	if(_clientHeader.isFullyRead()== false)
	{
		std::cout << "WARNING: Client Response is created with header that is not read yet" << std::endl;
	}
	Logger::info("Server setting of client"); std::cout << std::endl;
	std::cout << server << std::endl;
	Logger::info("Header of client"); std::cout<<std::endl;
	std::cout << header << std::endl;
	Logger::info("Full message of client: "); std::cout << header.getFullMessage() << std::endl;
}

ClientResponse::ClientResponse(const ClientResponse& source)
:_clientHeader(source._clientHeader), _server(source._server)
{

}

ClientResponse& ClientResponse::operator=(const ClientResponse& source)
{
	(void)source;
	return (*this);
}

ClientResponse::~ClientResponse()
{

}

void ClientResponse::sendSimpleResponse()const 
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

std::ostream& operator<<(std::ostream& os, const ClientResponse& response)
{
	os<<"Client Response header: " << std::endl;
	os << response._clientHeader << std::endl;
	os<<"Client server : " << std::endl;
	os << response._server << std::endl;
	return os;
}
