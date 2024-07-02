#include "ClientResponse.hpp"
#include "ClientHeader.hpp"
#include <ostream>
#include <iostream>


ClientResponse::ClientResponse(ClientHeader& header,
	const ServerSettings& server)
:_clientHeader(header), _server(server)
{
	if(_clientHeader.isFullyRead()== false)
	{
		std::cout << "WARNING: Client Response is created with header that is not read yet" << std::endl;
	}
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

std::ostream& operator<<(std::ostream& os, const ClientResponse& response)
{
	os<<"Client Response header: " << std::endl;
	os << response._clientHeader << std::endl;
	os<<"Client server : " << std::endl;
	os << response._server << std::endl;
	return os;
}
