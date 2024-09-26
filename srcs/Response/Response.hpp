#ifndef RESPONSE_HPP
# define RESPONSE_HPP
#include <ostream>
#include "ResponseHeader.hpp"
#include "ResponseBody.hpp"

class Client;
//#include "../Parsing/VirtualServer.hpp"

class Response 
{
	private :
		const Client& _client;
		const VirtualServer* _server; //This class is not resposible for deleting
		ResponseHeader* _responseHeader; //owning
		//CLASS HEADER;
		ResponseBody _responseBody;
		size_t		_bytesSent;
		
		//std::string _body; 
		//Response();
		std::string _createResponseString(void);

	public :
		Response(const Client& client, const VirtualServer* server);
		Response(const Response& source);
		Response& operator=(const Response& source);
		~Response();

		std::string getResponseString(void);
		void sendSimpleResponse() const;
		bool sendResponse();
		size_t	getBytesSent();

		friend std::ostream& operator<<(std::ostream& os, const Response& obj);
};

#endif
