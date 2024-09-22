# ifndef IO_HPP
# define IO_HPP

# include "../Client/Client.hpp"

# define MAXLINE 4096


class Message;

class Io {
	public:
		// Methods
		void	ioLoop();
		// Attributes

	private:
		// Methods
		void	_ioClient(Client& client);	
		 void	_sendMsg(Client& client);
		void	_receiveMsg(Client& client, Client::fdStatePair& fdPair, Message* message);

		// Attributes
		char*	_buffer;
};

# endif