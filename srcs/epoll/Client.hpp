
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
		void				addRecvLineToMessage();
		void				setNoWrite();
		bool				check_timeout() const;

							//Client specific functions

	private:
		unsigned long const	_id;
		int const			_fd;
		std::clock_t const	_start;
		int const			_epollfd;
		int					_write;
		std::string			_message;
		uint8_t*			_recvline;
							Client(void);
};

#endif

