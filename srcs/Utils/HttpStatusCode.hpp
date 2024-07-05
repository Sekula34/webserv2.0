#ifndef HTTPSTATUSCODE_HPP
# define HTTPSTATUSCODE_HPP
#include <exception>
#include <map>
#include <string>

class HttpStatusCode 
{
	private : 
		static std::map<int, std::string> _getFilledMap();
		static std::map<int, std::string> _statusCode;

	public:
		static std::string getReasonPhrase(int code);

		class UnkownCodeException : public std::exception
		{
			public :
				const char * what() const throw();
		};
};

#endif
