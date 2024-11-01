#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <netinet/in.h>
# include <vector>

const int backlog(10); //max of 10 connection on listen queue

class Socket 
{
	public:
		// Methods
		static std::vector<Socket>&	getSockets();
		// static std::vector<int>&	getSocketFds();
		static void					closeSockets();
		// int getCommunicationSocket(void);
		const int&					getSocketFd() const;
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

	public:
		void printSocketInfo(void);
};

#endif
