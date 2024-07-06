#ifndef  RESPONSEBODY_HPP
# define RESPONSEBODY_HPP
#include "../Client/ClientHeader.hpp"
#include "../Parsing/ServerSettings.hpp"

class ResponseBody
{
	private :
		const ClientHeader& _clientHeader;
		const ServerSettings* _server; //do not delete here


	public :
		std::string _generateErrorPage(const int httpErrorCode);
		//ResponseBody();
		ResponseBody(const ClientHeader& clientHeader, const ServerSettings* server = NULL);
		ResponseBody(const ResponseBody& source);
		ResponseBody& operator=(const ResponseBody& source);
		~ResponseBody();

};

#endif
