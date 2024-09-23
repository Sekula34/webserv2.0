# ifndef IO_HPP
# define IO_HPP

# include "../Client/Client.hpp"

# define MAXLINE 4096


class Message;

class Io
{
	public:
		// Methods
		void	ioLoop();
		// Attributes

	private:
		// Methods
		void	_ioClient(Client& client);	
		//  void	_sendMsg(Client& client);
		void	_sendMsg(Client& client, FdData& fdData, Message* message);
		void	_receiveMsg(Client& client, FdData& fdData, Message* message);

		// Attributes
		char*	_buffer;
	
	public:
		Io();
		~Io();
		Io(const Io& src);
		Io&		operator=(const Io& src);
};

# endif
