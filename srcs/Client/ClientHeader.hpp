#ifndef CLIENTHEADER_HPP
# define CLIENTHEADER_HPP
#include <string>
#include <map>
#include <vector>
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
 * @brief class for reading and storing client Message, one read at the time
 * 
 */
class ClientHeader 
{
	public :
		ClientHeader(const std::string message);
		ClientHeader(const ClientHeader& source);
		ClientHeader& operator=(const ClientHeader& source);
		~ClientHeader();


		UrlSuffix* urlSuffix; // this class is resposible for this 
		const int& getHostPort(void) const;
		const std::string& getHostName(void) const;
		const std::string& getFullMessage(void) const;
		const int& getErrorCode(void) const;
		const RequestLine& getRequestLine() const;
		const std::string& getRequestedUrl() const;
		const std::map<std::string, std::string> & getHeaderFields() const; 
		const std::string& getURLSuffix() const;
		bool isBodyExpected() const;

	private :
		ClientHeader();
		std::map<std::string, std::string> _headerFields;
		const std::string _message;
		int _errorCode;
		std::string _requestLine;
		RequestLine _requestLineElements;
		Host _host;


		void _constructFunction();
		bool _setCHVarivables();
		bool _setHeaderFields();


		/**
		 * @brief get vector of unseperated header values withut first line and last two CRLF
		 * 
		 * @return std::vector<std::string> 
		 */
		std::vector<std::string> _getHeaderFields(void) const;

		void _setOneHeader(std::string keyAndValue);
		void _initAllVars(void);
		/**
		 * @brief set Client header variables
		 * 
		 * @return true if evertyhing is settet
		 * @return false and _error code is 0 if it needs to be read more, false and _error code differnt than 0 if bad request
		 */
		/**
		 * @brief tries to set request line.
		 * 
		 * @return true if there is no error
		 * @return false if there is error, _error code is setted
		 */
		bool _setRequestLine(void);

		/**
		 * @brief fill Request struct
		 * 
		 * @return true if request struct is setted
		 * @return false if error occures, _error code is setted
		 */
		bool _fillRequestStruct();


		 /**
		  * @brief check request struct
		  * 
		  * @return true if everyhing ok
		  * @return false if not and set _error
		  */
		bool _checkRequestStruct(void);

		 /**
		  * @brief set host
		  * 
		  * @return true if succesfull
		  * @return false and set _error code if not
		  */
		bool _setHost(void);


		friend class CHTest;
		friend std::ostream& operator<<(std::ostream& os, const ClientHeader& obj);
};

#endif
