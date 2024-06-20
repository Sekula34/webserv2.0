#ifndef SOCKET_HPP
# define SOCKET_HPP
# include <sys/socket.h>
# include <netinet/in.h>

const int backlog(10); //max of 10 connection on listen que


/**
 * @brief contains all necessery information to create socket that will listen on portNumber
 only importat is public function that returns socket file descriptor getSocketFd
 * 
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
};

#endif
