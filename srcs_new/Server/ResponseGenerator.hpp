#ifndef RESPONSE_GENERATOR
# define RESPONSE_GENERATOR
#include "../Client/Client.hpp"
#include "../Utils/FileUtils.hpp"


class ResponseGenerator
{
	typedef FileUtils::e_fileType fileType;
	public: 
		static void		generateClientResponse(Client& client);

		const std::string& getResponse() const;
		const int&		getResponseHttpStatus() const;

					ResponseGenerator(Client& client);
					ResponseGenerator(const ResponseGenerator& source);
					~ResponseGenerator();

	private:
		void 		_responseMenu();
		std::string _renderServerErrorPage(int errorCode);
		std::string _generateErrorPage(const int httpErrorCode);
		void 		_setFileType(fileType type);

		Client&		_client;
		std::string	_response;
		int			_httpStatus;
		fileType	_fileType;

};

#endif
