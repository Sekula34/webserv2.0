#ifndef CLIENTRESPONSEMANAGER_HPP
# define CLIENTRESPONSEMANAGER_HPP
#include "ClientResponse.hpp"
#include <vector>

/**
 * @brief class for handling multiple clientResponses. Should be used by Dispatcher
 * 
 */
class ClientResponseManager
{
	private :
		std::vector<ClientResponse> _responses;

	public :
		ClientResponseManager();
		ClientResponseManager(const ClientResponseManager& source);
		ClientResponseManager& operator=(const ClientResponseManager& source);
		~ClientResponseManager();

		void addResponse(ClientResponse& clientResponse);

};

#endif
