#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
#include <exception>
#include <ostream>
#include <string>


/**
 * @brief struct that contains all info about requstLine
 * 
 */
struct RequestLine
{
	std::string requestMethod;
	std::string requestTarget;
	std::string protocolVersion;
};

/**
 * @brief struct that contains example localhost as name and 8080 as port
 * 
 */
struct Host 
{
	std::string name;
	int port;
};

/**
 * @brief class that contains information for client requst
 * @throw InvalidClientRequest exception if request is invalid and set _error that you can get with e.getErrorCode
 */
class ClientRequest
{
	private :
		std::string _request;
		std::string _requestLine;
		RequestLine _requestLineElements;
		Host _host;

		void _setRequestLine(void);
		void _fillRequestStruct();
		void _checkRequestStruct(void);

		void _setHost(void);
		ClientRequest();

	public:
		ClientRequest(std::string fullContent);
		ClientRequest(const ClientRequest& source);
		ClientRequest& operator=(const ClientRequest& source);
		~ClientRequest();

		friend std::ostream& operator<<(std::ostream& os, const ClientRequest& obj);
		class InvalidClientRequestException : public std::exception
		{
			public :
				InvalidClientRequestException(int errorCode, const std::string& errorMessage);
				~InvalidClientRequestException() throw() {}
				int getErrorCode() const;
				const char * what() const throw();
			private :
				int _errorCode;
				std::string _errorMessage;
		};

};

#endif
