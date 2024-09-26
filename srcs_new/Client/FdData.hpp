# ifndef FDDATA_HPP
# define FDDATA_HPP



class FdData
{
	public:
		enum	e_fdType
		{
			CLIENT_FD = 0,
			TOCHILD_FD = 1,
			FROMCHILD_FD = 2
		};

		enum	e_fdState
		{
			NEW,		// ready to be added to epoll
			NONE,		// FD is in epoll but not ready for send or receive 
			R_RECEIVE,	// fd ready for receive
			R_SEND,		// fd ready for send
			R_SENDREC,	// fd ready for send and receive
			CLOSE,		// close fd
			CLOSED		// fd is closed
		};

		// Methods
		// Attributes
		const int				fd;
		const e_fdType		 	type;
		e_fdState				state;

		struct findFd
        { 
			int fd;
			findFd(int fd)
			:fd(fd) {}

			bool operator()(const FdData& fds)
			{
				return (fds.fd == fd);
			}
        };
		struct findType
        { 
			e_fdType type;
			findType(e_fdType type)
			:type(type) {}

			bool operator()(const FdData& fds)
			{
				return (fds.type == type);
			}
        };

	private:
		// Methods
		// Attributes
	public:
		FdData(int fd, e_fdType type);
		~FdData(void);
		FdData(FdData const & src);

	private:
						FdData(void);
		FdData &		operator=(FdData const & rhs);
};


# endif
