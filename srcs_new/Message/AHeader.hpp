#ifndef AHEDER_HPP
# define AHEDER_HPP
#include <ostream>
#include <string>
#include <map>
#include <vector>


class HeaderTester;


class AHeader
{
	public:
		AHeader(int& errorCode);
		AHeader(const std::string& headerSection, int& errorCode);
		AHeader(const AHeader& source);
		AHeader& operator=(const AHeader& source);
		virtual ~AHeader();

		const std::map<std::string, std::string>&	getHeaderFieldMap() const;
		const int&									getHttpStatusCode(void) const;
		bool										isBodyExpected() const;
		void										setOneHeaderField(std::string key, std::string value);

	protected:
		std::map<std::string, std::string>	m_headerFields;
		const std::string					m_headerSection; //without start line in string; expected to be delimited by /r/n
		int&								m_errorCode;
		virtual std::string					getStartLine() const = 0;
		void								p_setHttpStatusCode(int httpCode);
		bool								p_isHeaderField(const std::string& field) const;
		std::string							p_getAllHeaderFieldsAsString() const;

	private:
		bool								_fillHeaderFieldMap(std::vector<std::string> plainHeaders);
		std::vector<std::string>			_getHeaderFields(const std::string& header_str) const;
		bool								_setOneHeaderField(std::string keyAndValue);
		bool								_checkHeaderFields(void);
		std::string							_getOneHeaderFieldAsString(std::string key, std::string value) const;
		friend class Message;

	public :
		friend std::ostream& operator <<(std::ostream& os, const AHeader& header);
};



#endif
