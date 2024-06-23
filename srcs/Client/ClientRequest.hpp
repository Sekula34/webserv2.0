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

class ClientRequest
{
	private :
		std::string _request;
		std::string _requestLine;
		RequestLine _requestLineElements;

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
				const char * what() const throw();
		};

};

#endif
