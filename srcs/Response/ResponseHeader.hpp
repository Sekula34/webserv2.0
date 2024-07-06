#ifndef RESPONSEHEADER_HPP
# define RESPONSEHEADER_HPP
#include <ostream>
#include <string>
#include <map>

struct StatusLineElements
{
	std::string HttpVersion;
	int statusCode;
	std::string ReasonPhrase;
};

class ResponseHeader
{
	private:
		int& _httpCode;
		StatusLineElements _statusLine;
		std::map<std::string, std::string> _headerFields;

		void _fillStatusLineElements();
		std::string _getStatusLineAsString() const;
		std::string _getOneHeaderFieldAsString(std::string key, std::string value) const;
		std::string _getAllHeaderFieldsAsString() const;

	public:
		ResponseHeader(int& httpCode);
		ResponseHeader(const ResponseHeader& source);
		ResponseHeader& operator=(const ResponseHeader& source);
		~ResponseHeader();

		std::string turnResponseHeaderToString(void) const;

		friend std::ostream& operator<<(std::ostream& os, const ResponseHeader& obj);

		/*void setContentLength(int contentLength);
		void setDate(void);
		void setContentLanguage(std::string language = "en");
		*/
};

#endif
