# ifndef FDDATA_HPP
# define FDDATA_HPP



class FdData
{
	enum	e_clientFdType
	{
		CLIENT_FD = 0,
		TOCHILD_FD = 1,
		FROMCHILD_FD = 2
	};

	enum	e_fdState
	{
		NONE, // Initial state of fd
		R_RECEIVE, // fd ready to be read
		R_SEND, // fd ready to be writted to
		R_SENDREC, // fd ready to be written to and read from
		CLOSE, // close fd
		CLOSED // fd is closed
	};

	public:
		// Methods
		// Attributes
		const int				fd;
		const e_clientFdType 	type;
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
			e_clientFdType type;
			findType(e_clientFdType type)
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
		FdData(int fd, e_clientFdType type);
		~FdData(void);

	private:
						FdData(void);
						FdData(FdData const & src);
		FdData &		operator=(FdData const & rhs);
};


# endif