
#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "poll.hpp"
# include <iostream>
# include <ctime>

class Client {

	public:
		static int			client_cntr;
		
							// canonical
							Client (int fd);
							~Client(void);
							Client(Client const & src);
		Client &			operator=(Client const & rhs);
							
							// set and get
		unsigned long		getId() const;
		int					getFd() const;
		std::clock_t		getStartTime() const;
		std::string			getMessage() const;
		void				addToMessage(char* buffer);
		void				setNoWrite();
		bool				check_timeout(std::clock_t time) const;

							//Client specific functions

	private:
		unsigned long		_id;
		int const			_fd;
		int					_write;
		std::string			_message;
		std::clock_t const	_start;
							Client(void);
};

#endif

