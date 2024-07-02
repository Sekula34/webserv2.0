#include "ClientResponseManager.hpp"
#include "ClientResponse.hpp"

ClientResponseManager::ClientResponseManager()
{

}

ClientResponseManager::ClientResponseManager(const ClientResponseManager& source)
:_responses(source._responses)
{

}

ClientResponseManager& ClientResponseManager::operator=
	(const ClientResponseManager& source)
{
	_responses = source._responses;
	return (*this);
}

ClientResponseManager::~ClientResponseManager()
{

}

void ClientResponseManager::addResponse(ClientResponse& clientResponse)
{
	_responses.push_back(clientResponse);
}
