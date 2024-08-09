#ifndef CLIENTREQUESTHEADER_HPP
# define CLIENTREQUESTHEADER_HPP
#include <string>

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
class ClientRequestHeader 
{
	public :
		ClientRequestHeader(const std::string message);
		ClientRequestHeader(const ClientRequestHeader& source);
		ClientRequestHeader& operator=(const ClientRequestHeader& source);
		~ClientRequestHeader();

		bool _setCHVarivables();

		const int& getHostPort(void) const;
		const std::string& getHostName(void) const;
		const std::string& getFullMessage(void) const;
		const int& getErrorCode(void) const;
		const RequestLine& getRequestLine() const;
		const std::string& getRequestedUrl() const;

	private :
		ClientRequestHeader();
		const std::string _message;
		int _errorCode;
		std::string _requestLine;
		RequestLine _requestLineElements;
		Host _host;


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

		friend std::ostream& operator<<(std::ostream& os, const ClientRequestHeader& obj);
};

#endif
