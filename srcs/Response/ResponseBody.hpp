#ifndef  RESPONSEBODY_HPP
# define RESPONSEBODY_HPP
#include "../Parsing/ServerSettings.hpp"

class Client;

class ResponseBody
{
	public :
		//ResponseBody();
		ResponseBody(const Client& client, const ServerSettings* server);
		ResponseBody(const ResponseBody& source);
		ResponseBody& operator=(const ResponseBody& source);
		~ResponseBody();

		const std::string& getResponse(void) const;
		const int& getHttpStatusCode(void) const;

	private :
		const Client& _client;
		const ServerSettings* _server; //do not delete here This class is not resposible for deleting this
		std::string _response;
		int _httpStatusCode;
		
		std::string _generateErrorPage(const int httpErrorCode);
		void _generateServerResponse();
		void _renderServerErrorPage(int errorCode);

		void _fetchServerPage(const LocationSettings& location);

		void _handlerGetMethod();
		void _handleRedirect(const NginnxReturn& redirect);

		void _processRequestedLocation(const LocationSettings& location);
		void _generateHtml(void);
		bool _setFilePath(std::string &filePath, const LocationSettings& location) const;


};

#endif
