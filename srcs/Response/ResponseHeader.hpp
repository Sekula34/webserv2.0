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
	private:
		const int& _httpCode;
		StatusLineElements _statusLine;
		void _fillStatusLineElements();
		std::string _getStatusLineAsString() const;

	public:
		ResponseHeader(const int& httpCode, size_t contentLength);
		ResponseHeader(const ResponseHeader& source);
		ResponseHeader& operator=(const ResponseHeader& source);
		~ResponseHeader();

		std::string getStartLine() const;
		std::string turnResponseHeaderToString(void) const;

		friend std::ostream& operator<<(std::ostream& os, const ResponseHeader& obj);

		/*void setContentLength(int contentLength);
		void setDate(void);
		void setContentLanguage(std::string language = "en");
		*/
};

#endif
