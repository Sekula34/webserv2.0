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
		void _setRequestLine(void);
		void _fillRequestStruct();
		void _checkRequestStruct(void);
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
		bool isFullyRead() const;

		/**
		 * @brief sets requstLine, requestLineElements, host and check if request is valid
		 * throw INVALIDCLIENTREQUESTEXCEPTIOn with code if something went wrong
		 * 
		 */
		void setCHVarivables();

		const int& getHostPort(void) const;
		const std::string& getHostName(void) const;

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
