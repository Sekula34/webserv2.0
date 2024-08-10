
#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "poll.hpp"
# include <iostream>
# include <ctime>

class Client {

	public:
		static int			client_cntr;
		
							// canonical
							Client (int const fd, int const epollfd);
							~Client(void);
							Client(Client const & src);
		Client &			operator=(Client const & rhs);
							
							// set and get
		unsigned long		getId() const;
		int					getFd() const;
		std::clock_t		getStartTime() const;
		std::string			getMessage() const;
		uint8_t*			getRecvLine() const;
		int					getEpollFd() const;
		bool				getReadHeader() const;
		bool				getReadBody() const;
		bool				getWriteClient() const;
		void				setReadHeader(bool b);
		void				setReadBody(bool b);
		void				setWriteClient(bool b);

							//Client specific functions
		void				addRecvLineToMessage();
		bool				check_timeout() const;

	private:
		unsigned long const	_id;
		int const			_fd;
		std::clock_t const	_start;
		int const			_epollfd;
		std::string			_message;
		uint8_t*			_recvline;
		bool				_readheader;
		bool				_readbody;
		bool				_writeclient;
							Client(void);
};

#endif

