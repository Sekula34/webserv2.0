
#ifndef CLIENT_HPP
# define CLIENT_HPP
# include <string>
# include <ctime>
// #define MAXLINE			4096
// #define MAXLINE			493
#define MAXLINE			50
#define MAX_TIMEOUT		10000

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
		unsigned char*			getRecvLine() const;
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
		unsigned char*			_recvline;
		bool				_readheader;
		bool				_readbody;
		bool				_writeclient;
							Client(void);
};

#endif
