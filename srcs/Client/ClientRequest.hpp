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
 * @brief class that contains information for client requst
 * if _error is settet server should immmediatily send that code response to client
 * @throw InvalidClientRequest exception if request is invalid and set _error
 */
class ClientRequest
{
	private :
		std::string _request;
		std::string _requestLine;
		RequestLine _requestLineElements;
		int _error;

		void _setRequestLine(void);
		void _fillRequestStruct();
		void _checkRequestStruct(void);
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
