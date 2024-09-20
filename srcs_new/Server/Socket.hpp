#ifndef SOCKET_HPP
# define SOCKET_HPP
// # include <ostream>
# include <sys/socket.h>
# include <netinet/in.h>
# include <vector>


const int backlog(10); //max of 10 connection on listen queue

class Socket 
{
	public:
		// Methods
		static const std::vector<int>	getSocketFDs(void);
		// int getCommunicationSocket(void);
		// friend std::ostream& operator<<(std::ostream& os, const Socket& socket);

	private:
		// Methods

		// Attributes
		static std::vector<Socket>	_allSockets;
		const int					_port;
		int							_socketFD;
		sockaddr_in					_adress;
		int							_addrlen; //= sizeof(adress);

	public:
		Socket(int portNumber); // Custom constructor
		Socket(const Socket& source); // This is used when pushed into the vector.
		Socket& operator=(const Socket& source);
		~Socket();

	private:
		Socket();

	// FIXME: FUNTIONS FOR TESTING OR NOT BEING USED
	public:
		void printSocketInfo(void);
};

#endif
