#ifndef RESPONSE_HPP
# define RESPONSE_HPP
#include <ostream>
#include <string>

class Response 
{
	private :
		int& _clientFd;
		int _code;
		//CLASS HEADER;
		std::string _body; 
		//Response();

	public :
		Response(int& _clientFd);
		Response(const Response& source);
		Response& operator=(const Response& source);
		~Response();

		friend std::ostream& operator<<(std::ostream& os, const Response& obj);
};

#endif
