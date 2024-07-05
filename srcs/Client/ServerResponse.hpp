#ifndef CLIENTRESPONSE_HPP
# define CLIENTRESPONSE_HPP
#include "ClientHeader.hpp"
#include "../Parsing/ServerSettings.hpp"
#include <ostream>


/**
 * @brief class that will contain client Header and server that will generate resposne to clientRequest(header)
 * 
 */
class ServerResponse 
{
	private :
		ClientHeader& _clientHeader;
		const ServerSettings& _server;


		//ServerResponse();
	public:
		ServerResponse(ClientHeader& header, const ServerSettings& server);
		ServerResponse(const ServerResponse& source);
		ServerResponse& operator=(const ServerResponse& source);
		~ServerResponse();

		void sendSimpleResponse(void) const;

		friend std::ostream& operator<<(std::ostream& os, const ServerResponse& response);
		
};

#endif
