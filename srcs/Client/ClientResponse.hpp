#ifndef CLIENTRESPONSE_HPP
# define CLIENTRESPONSE_HPP
#include "ClientHeader.hpp"
#include "../Parsing/ServerSettings.hpp"
#include <ostream>


/**
 * @brief class that will contain client Header and server that will generate resposne to clientRequest(header)
 * 
 */
class ClientResponse 
{
	private :
		ClientHeader& _clientHeader;
		const ServerSettings& _server;


		//ClientResponse();
	public:
		ClientResponse(ClientHeader& header, const ServerSettings& server);
		ClientResponse(const ClientResponse& source);
		ClientResponse& operator=(const ClientResponse& source);
		~ClientResponse();

		void sendSimpleResponse(void) const;

		friend std::ostream& operator<<(std::ostream& os, const ClientResponse& response);
		
};

#endif
