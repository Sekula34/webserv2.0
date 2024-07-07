#ifndef RESPONSE_HPP
# define RESPONSE_HPP
#include <ostream>
#include "ResponseHeader.hpp"
#include "../Client/ClientHeader.hpp"
#include "ResponseBody.hpp"
//#include "../Parsing/ServerSettings.hpp"

class Response 
{
	private :
		ClientHeader& _clientHeader;
		const ServerSettings& _server;
		ResponseHeader* _responseHeader;
		//CLASS HEADER;
		ResponseBody _responseBody;
		
		//std::string _body; 
		//Response();

	public :
		Response(ClientHeader& clientHeader, const ServerSettings& server);
		Response(const Response& source);
		Response& operator=(const Response& source);
		~Response();

		void sendSimpleResponse() const;

		friend std::ostream& operator<<(std::ostream& os, const Response& obj);
};

#endif
