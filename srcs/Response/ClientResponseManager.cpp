#include "ClientResponseManager.hpp"

ClientResponseManager::ClientResponseManager()
{

}

ClientResponseManager::ClientResponseManager(const ClientResponseManager& source)
//_responses(source._responses)
{
	(void) source;
}

ClientResponseManager& ClientResponseManager::operator=
	(const ClientResponseManager& source)
{
	(void) source;
	//_responses = source._responses;
	return (*this);
}

ClientResponseManager::~ClientResponseManager()
{

}

// void ClientResponseManager::addResponse(ServerResponse& ServerResponse)
// {
// 	_responses.push_back(ServerResponse);
// }
