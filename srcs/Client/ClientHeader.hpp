#ifndef CLIENTHEADER_HPP
# define CLIENTHEADER_HPP
#include <string>

/**
 * @brief enum used for return read Status 
 * 
 */
enum ReadStatus
{
	CLIENT_CLOSE = -2,
	ERROR = -1,
	DONE,
	CONTINUE_READING
};


const int BUFFER_SIZE = 4096;


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

	private :
		int& _clientFd;
		std::string _message;
		bool _fullyRead;
		int _errorCode;

		std::string _requestLine;
		RequestLine _requestLineElements;
		Host _host;
		ClientHeader();

		bool _isConnectionClosedByClient(void);

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

	public :
		ClientHeader(int& clientFd);
		ClientHeader(const ClientHeader& source);
		ClientHeader& operator=(const ClientHeader& source);
		~ClientHeader();

		/**
		 * @brief 
		 * 
		 * @return ReadStatus DONE if END OF HEADER IS RECEIVED, CONTINUE READING if buffer size is full but end of header is not recevied
		 * ERROR if read Failes or received message is without header
		 */
		ReadStatus readOnce();

		const int& getClientFd() const;
		/**
		 * @brief return true if clientRequestHeader is fully read, return false if not
		 * 
		 * @return true 
		 * @return false 
		 */
		bool isFullyRead() const;

		/**
		 * @brief set Client header variables
		 * 
		 * @return true if evertyhing is settet
		 * @return false and _error code is 0 if it needs to be read more, false and _error code differnt than 0 if bad request
		 */
		bool setCHVarivables();

		const int& getHostPort(void) const;
		const std::string& getHostName(void) const;
		const std::string& getFullMessage(void) const;
		const int& getErrorCode(void) const;
		const RequestLine& getRequestLine() const;

		friend std::ostream& operator<<(std::ostream& os, const ClientHeader& obj);

		class InvalidClientRequestException : public std::exception
		{
			public :
				InvalidClientRequestException(int errorCode, const std::string& errorMessage);
				~InvalidClientRequestException() throw() {}
				int getErrorCode() const;
				const char * what() const throw();
			private :
				int _errorCode;
				std::string _errorMessage;
		};

};


class FindClientByFd {

private:
    int search_fd;
public:
    FindClientByFd(int fd) : search_fd(fd) {}

	//this make class a functor(class that defines () method )
    bool operator()(const ClientHeader& clientheader) const
	{
        return clientheader.getClientFd() == search_fd;
    }
};
#endif
