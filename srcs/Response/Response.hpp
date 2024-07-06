#ifndef RESPONSE_HPP
# define RESPONSE_HPP
#include <ostream>
#include <string>
#include "ResponseHeader.hpp"

class Response 
{
	private :
		int& _clientFd;
		const ResponseHeader& _header;
		int _code;
		//CLASS HEADER;
		std::string _body; 
		//Response();

	public :
		Response(int& _clientFd, const ResponseHeader& header);
		Response(const Response& source);
		Response& operator=(const Response& source);
		~Response();

		friend std::ostream& operator<<(std::ostream& os, const Response& obj);
};

#endif
