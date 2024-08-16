
#ifndef EPOLLHANDLER_HPP
# define EPOLLHANDLER_HPP

# include <iostream>
# include "sys/epoll.h"
# include "../epoll/Client.hpp"

class Client;

class EpollHandler {

	public:
					// canonical
					EpollHandler(void);
		virtual		~EpollHandler(void);
					EpollHandler(EpollHandler const & src);
		EpollHandler &	operator=(EpollHandler const & rhs);

					// set and get

					//EpollHandler specific functions
		static void	epoll_add_fd(int epollfd, int clientfd);

	private:
};

#endif

