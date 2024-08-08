# include "epoll.hpp"
# include "Socket.hpp"


void	exit_me(std::string s)
{
	std::cout << s << std::endl;
	exit (EXIT_FAILURE);
}

int	init_epoll(void)
{
	int epollfd = epoll_create(1);
	// CREATING EPOLL
	if (epollfd == -1)					
		exit_me("epoll_create error");
	return (epollfd);
}

void epoll_add_listen_sock(int listen_sock, int epollfd)
{
    struct epoll_event	ev;
    // SETTING UP EV EVENTS STRUCT FOR LISTEN_SOCKET
	ev.events = EPOLLIN;				
	ev.data.fd = listen_sock;

	// ADDING LISTEN_SOCKET TO EPOLL WITH THE EV 'SETTINGS' STRUCT
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) 
		exit_me("epoll_ctl_add - listen socket error");
}



// int	init_socket()
// {
// 	int opt(1);

// 	struct sockaddr_in	servaddr;
// 	int	listen_socket;

// 	if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
// 		exit_me("socket error");
	
// 	bzero(&servaddr, sizeof(servaddr));
// 	servaddr.sin_family = AF_INET;
// 	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
// 	servaddr.sin_port = htons(SERVER_PORT);

//     int retVal;
//     retVal = setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
// 	if (retVal == -1)
// 		exit_me("setsockopt error");
// 	if ((bind(listen_socket, (SA *) &servaddr, sizeof(servaddr))) < 0)
// 		exit_me("bind error");

// 	if ((listen(listen_socket, MAX_LISTEN)) < 0)
// 		exit_me("listen error");
// 	return (listen_socket);
// }

void	epoll_add_client(int epollfd, int listen_socket)
{
	struct epoll_event	ev;
	int	conn_socket;

	conn_socket = accept(listen_socket, (SA *) NULL, NULL);
	if (conn_socket == -1)
		exit_me("accept error");
	std::cout << "accepted conn_socket: " << conn_socket << std::endl;
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = conn_socket;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_socket, &ev) == -1)
		exit_me("epoll_ctl - conn_socket error");
}

void	handle_client(int epollfd, int connfd, struct epoll_event* events,
				   std::map<int, std::string> & messages, int idx)
{
	std::string	answer = "HTTP/1.0 200 OK\r\n\r\nWebserv 0.0";
	uint8_t		recvline[MAXLINE];
	int			n = 0;
	int			peek = 0;

	memset(recvline, 0, MAXLINE);
	if (events[idx].events & EPOLLIN)
		n = recv(connfd, recvline, MAXLINE - 1, MSG_DONTWAIT);
	else
		return ;
	if (n > 0)
	{
		messages[connfd] += (char *)recvline;
		memset(recvline, 0, MAXLINE);
		if (n == MAXLINE && messages[connfd].find("\r\n\r\n") == std::string::npos)
			peek = recv(connfd, recvline, MAXLINE, MSG_PEEK | MSG_DONTWAIT);
	}
	std::cout << "+++++ FD: "<< events[idx].data.fd << " , FLAG: "<< events[idx].events << std::endl;
	if (n < 0 || peek < 0)
		exit_me("read error");
	if (n <= MAXLINE && messages[connfd].find("\r\n\r\n") != std::string::npos)
	{
		std::cout << std::endl << messages[connfd] << std::endl;
		if (messages.find(connfd) != messages.end())
			messages[connfd].erase();
		if (events[idx].events & EPOLLOUT)
		{
			write(connfd, answer.c_str(), answer.size());
			epoll_ctl(epollfd, EPOLL_CTL_DEL, connfd, events);
			close (connfd);
		}
	}
}

int epoll_loop(void)
{
	struct epoll_event	events[MAX_EVENTS];
	std::map<int, std::string>	messages;
	int	listen_socket;
	int	nfds;
	int	epollfd;

	listen_socket = init_socket();
	epollfd = init_epoll(listen_socket);
	int i = 1;
	while (1)
	{
		std::cout << "going throught loop " << i << " times" << std::endl;
		std::cout << "waiting for a connection on port: " <<  SERVER_PORT << std::endl;
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, MAX_WAIT);
		if (nfds == -1)
			exit_me("epoll_wait error");
	
		for (int n = 0; n < nfds; ++n)
		{
			if (events[n].data.fd == listen_socket)
				epoll_add_client(epollfd, listen_socket);
			else
				// read_client
				// 	write_client
				handle_client(epollfd, events[n].data.fd, events, messages, n);
		}
		i++;
	}
}