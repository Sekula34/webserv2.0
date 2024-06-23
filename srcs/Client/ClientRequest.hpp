#ifndef CLIENTREQUEST_HPP
# define CLIENTREQUEST_HPP
#include <ostream>
#include <string>

class ClientRequest
{
	private :
		std::string _request;
		ClientRequest();

	public:
		ClientRequest(std::string fullContent);
		ClientRequest(const ClientRequest& source);
		ClientRequest& operator=(const ClientRequest& source);
		~ClientRequest();

		friend std::ostream& operator<<(std::ostream& os, const ClientRequest& obj);

};

#endif
