#ifndef SOCKET_HPP
# define SOCKET_HPP
# include <sys/socket.h>
# include <netinet/in.h>


const int backlog(10); //max of 10 connection on listen que


/**
 * @brief contains all necessery information to create socket that will listen on portNumber
 only importat is public function that returns socket file descriptor getSocketFd
 * call Socket::closeAllSocket for closing all _socketFD
 */
class Socket 
{
	private :
		const int _port;
		int _socketFD;
		sockaddr_in _adress;
		int _addrlen; //= sizeof(adress);

		Socket();
	public :
		Socket(int portNumber);
		Socket(const Socket& source);
		Socket& operator=(const Socket& source);
		~Socket();

		const int& getSocketFd(void) const;
		
		/**
		* @brief accept connection on this Socket an return communication socket
		* 
		* @return int new filedescriptor that is used to communicate with client
		* @note you must close this
		*/ 
		int getCommunicationSocket(void);
};

#endif
