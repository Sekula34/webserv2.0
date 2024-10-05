#ifndef REQUESTHEADER 
# define REQUESTHEADER
#include "AHeader.hpp"
#include <string>
#include "../Utils/UrlSuffix.hpp"


class CHTest;

/**
 * @brief struct that contains all info about requstLine
 * 
 */
struct RequestLine
{
	std::string requestMethod;
	std::string requestTarget;
	std::string protocolVersion;
};

/**
 * @brief struct that contains example localhost as name and 8080 as port
 * 
 */
struct Host 
{
	std::string name;
	int port;
};

/**
 * @brief class that replace old ClientHeader class 
 * 
 */
class RequestHeader : public AHeader
{
	public:
		RequestHeader(const std::string fullRequestHeader, int& errorCode);
		RequestHeader(const RequestHeader& source);
		RequestHeader& operator=(const RequestHeader& source);
		~RequestHeader();
		static bool	checkHeaderByLine(const std::string str);

		UrlSuffix* urlSuffix; // this class is resposible for this 
		
		std::string getStartLine() const;
		const int& getHostPort(void) const;
		const std::string& getHostName(void) const;
		const std::string& getFullMessage(void) const;

		const RequestLine& getRequestLine() const;
		const std::string& getURLSuffix() const;
		/**
		 * @brief Get the Full Client URL 
		 * 
		 * @return std::string for example http://localhost:8080/autoindex/first.html
		 */
		std::string getFullClientURL() const;


	private:

		Host _host;
		void _constructFunction();
		void _initRequestVars(void);
		bool _setReqVariables();

		static std::string getFullRequest(const std::string& message);
		static std::string getHeaderSectionString(const std::string& message);
		void _initFunction();
		const std::string _fullRequest;
		std::string _requestLine;
		RequestLine _requestLineElements;
		bool _setRequestLine(void);

		/**
		 * @brief fill Request struct
		 * 
		 * @return true if request struct is setted
		 * @return false if error occures, _error code is setted
		 */
		bool _fillRequestStruct();

		bool _setHost(void);


		 /**
		  * @brief check request struct
		  * 
		  * @return true if everyhing ok
		  * @return false if not and set _error
		  */
		bool _checkRequestStruct(void);
		friend std::ostream& operator<<(std::ostream& os, const RequestHeader& obj);

};

#endif 
