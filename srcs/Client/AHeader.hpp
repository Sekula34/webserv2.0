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
		AHeader();
		AHeader(const std::string& headerSection);
		AHeader(const AHeader& source);
		AHeader& operator=(const AHeader& source);
		//TODO: make destructor virtual
		virtual ~AHeader();

		const std::map<std::string, std::string>& getHeaderFieldMap() const;
		const int& getHttpStatusCode(void) const;
		bool isBodyExpected() const;
		void setOneHeaderField(std::string key, std::string value);

	protected:
		std::map<std::string, std::string> m_headerFields;
		const std::string m_headerSection; //without start line in string; expected to be delimited by /r/n
		int m_httpErrorCode;
		//TODO: uncomment this abstract function 
		virtual std::string getStartLine() const = 0;
		//virtual std::string getStartLine() = 0
		void p_setHttpStatusCode(int httpCode);
		std::string p_getAllHeaderFieldsAsString() const;

	private:
		bool _fillHeaderFieldMap();
		bool _setOneHeaderField(std::string keyAndValue);
		std::vector<std::string> _getHeaderFields() const;
		bool _checkHeaderFields(void);
		std::string _getOneHeaderFieldAsString(std::string key, std::string value) const;
		
		
		friend std::ostream& operator <<(std::ostream& os, const AHeader& header);
};



#endif
