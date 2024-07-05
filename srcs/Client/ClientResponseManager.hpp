#ifndef SERVERRESPONSE_HPP
# define SERVERRESPONSE_HPP
#include "ServerResponse.hpp"
#include <vector>

/**
 * @brief class for handling multiple clientResponses. Should be used by Dispatcher
 * 
 */
class ClientResponseManager
{
	private :
		std::vector<ServerResponse> _responses;

	public :
		ClientResponseManager();
		ClientResponseManager(const ClientResponseManager& source);
		ClientResponseManager& operator=(const ClientResponseManager& source);
		~ClientResponseManager();

		void addResponse(ServerResponse& ServerResponse);

};

#endif
