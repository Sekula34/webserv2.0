#ifndef RESPONSE_GENERATOR
# define RESPONSE_GENERATOR
#include "../Client/Client.hpp"
#include "../Utils/FileUtils.hpp"
#include "LocationSettings.hpp"


class LocationSettings;

class ResponseGenerator
{
	typedef FileUtils::e_fileType fileType;
	public: 
		static void			generateClientResponse(Client& client);
		const NginnxReturn& getRedirect() const;

		const std::string&	getResponse() const;
		const int&			getResponseHttpStatus() const;

							ResponseGenerator(Client& client);
							ResponseGenerator(const ResponseGenerator& source);
							~ResponseGenerator();

	private:
		void 		_responseMenu();
		std::string _renderServerErrorPage(int errorCode);
		std::string _renderLocationErrorPage(const LocationSettings& location, const int& errorCode);
		std::string _renderErrorPage(const std::string& errorPagePath, const int& errorCode);
		std::string _generateErrorPage(const int httpErrorCode);
		void 		_setFileType(fileType type);
		void 		_redirectHandler(const LocationSettings& location);
		void		_getHandler(const LocationSettings& location);
		void		_postHandler(const LocationSettings& location);
		void		_deleteHandler(const LocationSettings& location);
		void		_generateHtml(const LocationSettings& location);
		void 		_autoindexHtml(const std::string& serverFilePath, const LocationSettings& location);
		//void		_dirHtml(const std::string& file, const LocationSettings& location);
		void		_fakePostHandler(const LocationSettings& location);


		Client&		_client;
		std::string	_response;
		int			_httpStatus;
		fileType	_fileType;
		NginnxReturn _redirect;
	

};

#endif
