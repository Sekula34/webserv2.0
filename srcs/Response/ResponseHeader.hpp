#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP
#include "../Client/AHeader.hpp"
#include <cstddef>
#include <ostream>
#include <string>

struct StatusLineElements
{
	std::string HttpVersion;
	int statusCode;
	std::string ReasonPhrase;
};

class ResponseHeader : public AHeader
{

	public:
		ResponseHeader(std::string header, const int httpCode = 200);
		ResponseHeader(const int& httpCode, size_t contentLength);
		ResponseHeader(const ResponseHeader& source);
		ResponseHeader& operator=(const ResponseHeader& source);
		~ResponseHeader();

		std::string getStartLine() const;
		std::string turnResponseHeaderToString(void) const;
		static ResponseHeader* createCgiResponseHeader(std::string cgiResponse, std::string cgiHeaderDelimiter);
	
	private:
		const int _httpCode;
		StatusLineElements _statusLine;
		void _fillStatusLineElements();
		std::string _getStatusLineAsString() const;

		static bool _cgiStatusLine(std::string firstLine);
		static bool _setStatusLine(StatusLineElements& elem, std::string line);

		friend std::ostream& operator<<(std::ostream& os, const ResponseHeader& obj);
};

#endif
