#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP
#include "AHeader.hpp"
#include <ostream>
#include <string>

class ResponseGenerator;

struct StatusLineElements
{
	std::string HttpVersion;
	int statusCode;
	std::string ReasonPhrase;
};

//FIXME: there could be problem with http code 
class ResponseHeader : public AHeader
{

	public:
		ResponseHeader();
		ResponseHeader(std::string header, int& errorCode);
		//ResponseHeader(const int& httpCode, size_t contentLength);
		ResponseHeader(const ResponseHeader& source);
		ResponseHeader& operator=(const ResponseHeader& source);
		~ResponseHeader();

		std::string getStartLine() const;
		std::string turnResponseHeaderToString(void) const;
		static ResponseHeader* createCgiResponseHeader(std::string cgiResponse, int& clientError, std::string cgiHeaderFieldDelimiter = "\n", std::string cgiHeaderDelimiter = "\n\n");
		static ResponseHeader* createRgResponseHeader(const ResponseGenerator& rg);
		static void	cgiToHttpConversion(ResponseHeader* toReturn);
		
		void changeHttpCode(int newHttpCode);

	private:
		int _httpCode;
		StatusLineElements _statusLine;
		void _fillStatusLineElements();
		std::string _getStatusLineAsString() const;

		bool _cgiStatusLine() const;
		static bool _setStatusLine(StatusLineElements& elem, std::string line);

		friend std::ostream& operator<<(std::ostream& os, const ResponseHeader& obj);
};

#endif
