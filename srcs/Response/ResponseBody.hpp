#ifndef  RESPONSEBODY_HPP
# define RESPONSEBODY_HPP
#include "../Client/ClientHeader.hpp"
#include "../Parsing/ServerSettings.hpp"

class ResponseBody
{
	private :
		const ClientHeader& _clientHeader;
		const ServerSettings& _server; //do not delete here
		std::string _response;
		int _httpStatusCode;
		
		std::string _generateErrorPage(const int httpErrorCode);


	public :
		//ResponseBody();
		ResponseBody(const ClientHeader& clientHeader, const ServerSettings& server);
		ResponseBody(const ResponseBody& source);
		ResponseBody& operator=(const ResponseBody& source);
		~ResponseBody();

		const std::string& getResponse(void) const;
		const int& getHttpStatusCode(void) const;

};

#endif
