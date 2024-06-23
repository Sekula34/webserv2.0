#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
#include <exception>
#include <ostream>
#include <string>

class ClientRequest
{
	private :
		std::string _request;
		std::string _requestLine;

		void _setRequestLine(void);
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
