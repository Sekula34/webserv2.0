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
		void _fillStatusLineElements(int num = 200);
		std::string _getStatusLineAsString() const;

		static bool _setStatusLine(StatusLineElements& elem, std::string line);

		friend std::ostream& operator<<(std::ostream& os, const ResponseHeader& obj);
};

#endif
