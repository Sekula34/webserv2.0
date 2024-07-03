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

		std::string _requestLine;
		RequestLine _requestLineElements;
		Host _host;
		ClientHeader();

		bool _isConnectionClosedByClient(void);

		/**
		 * @brief set request line
		 * @throw InvalidClientRequestException(400, "BAD request") if cannot find one
		 * 
		 */
		void _setRequestLine(void);

		/**
		 * @brief fill the request method, request target(location) and protocol
		 * @throw InvalidClientRequestException(400, BAD REQUEST) if cannot split line in 3 elements
		 */
		void _fillRequestStruct();

		/**
		 * @brief check if method in Request is GET POST or delete
		 * @throw InvalidClientRequestException(405, "Method NOT ALLOWED")
		 * @throw InvalidClientRequestException(505, "HTTP version NOT SUPPORTED")
		 */
		void _checkRequestStruct(void);

		/**
		 * @brief set host structure, PORT and name 
		 * @throw InvalidClientRequsetExcpetion(400, "Bad request")
		 */
		void _setHost(void);

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
		 * @brief sets requstLine, requestLineElements, host and check if request is valid
		 * throw InvalidClientRequestException with code if something went wrong
		 * 
		 */
		void setCHVarivables();

		const int& getHostPort(void) const;
		const std::string& getHostName(void) const;
		const std::string& getFullMessage(void) const;

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
