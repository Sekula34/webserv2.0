#ifndef RESPONSE_HPP
# define RESPONSE_HPP
#include <ostream>
#include "ResponseHeader.hpp"
#include "ResponseBody.hpp"
#include "../epoll/Client.hpp"
//#include "../Parsing/ServerSettings.hpp"

class Response 
{
	private :
		const Client& _client;
		const ServerSettings* _server; //This class is not resposible for deleting
		ResponseHeader* _responseHeader; //owning
		//CLASS HEADER;
		ResponseBody _responseBody;
		
		//std::string _body; 
		//Response();
		std::string _createResponseString(void);

	public :
		Response(const Client& client, const ServerSettings* server);
		Response(const Response& source);
		Response& operator=(const Response& source);
		~Response();

		std::string getResponseString(void);
		void sendSimpleResponse() const;
		bool sendResponse();

		friend std::ostream& operator<<(std::ostream& os, const Response& obj);
};

#endif
