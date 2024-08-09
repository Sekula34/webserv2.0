# include "poll.hpp"

void	exit_me(std::string s)
{
	std::cout << s << std::endl;
	exit (EXIT_FAILURE);
}

int	init_epoll(int listen_sock)
{
	int epollfd = epoll_create(1);
	struct epoll_event	ev;

	// CREATING EPOLL
	if (epollfd == -1)					
		exit_me("epoll_create error");

	// SETTING UP EV EVENTS STRUCT FOR LISTEN_SOCKET
	ev.events = EPOLLIN | EPOLLOUT;				
	ev.data.fd = listen_sock;

	// ADDING LISTEN_SOCKET TO EPOLL WITH THE EV 'SETTINGS' STRUCT
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) 
		exit_me("epoll_ctl_add - listen socket error");
	return (epollfd);
}

int	init_socket()
{
	int opt(1);

	struct sockaddr_in	servaddr;
	int	listen_socket;

	if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit_me("socket error");
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);

    int retVal;
    retVal = setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	if (retVal == -1)
		exit_me("setsockopt error");
	if ((bind(listen_socket, (SA *) &servaddr, sizeof(servaddr))) < 0)
		exit_me("bind error");

	if ((listen(listen_socket, MAX_LISTEN)) < 0)
		exit_me("listen error");
	return (listen_socket);
}

void	epoll_add_client(int epollfd, int listen_socket, std::map<int, Client *>& clients)
{
	struct epoll_event	ev;
	int	client_fd;

	client_fd = accept(listen_socket, (SA *) NULL, NULL);
	if (client_fd == -1)
		exit_me("accept error");
	std::cout << "accepted conn_socket: " << client_fd << std::endl;
	Client * newClient = new Client(client_fd);
	if (clients.find(client_fd) != clients.end())
		clients[client_fd]->setNoWrite();
	clients[client_fd] = newClient;
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = client_fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
		exit_me("epoll_ctl - conn_socket error");
}

void	clients_remove(std::map<int, Client*> & clients, int client_fd)
{
	clients[client_fd]->setNoWrite();

	clients.erase(client_fd);
}

void	epoll_remove_client(int epollfd, struct epoll_event* events,
					std::map<int, Client*> & clients, int client_fd)
{
	clients_remove(clients, client_fd);
	epoll_ctl(epollfd, EPOLL_CTL_DEL, client_fd, events);
	close (client_fd);
}

void	handle_client(int epollfd, int client_fd, struct epoll_event* events,
				   std::map<int, Client *> & clients, int idx)
{
	std::string	answer = "HTTP/1.0 200 OK\r\n\r\nWebserv 0.0";
	uint8_t		recvline[MAXLINE];
	int			n = 0;
	int			peek = 0;

	if (clients.find(client_fd) == clients.end())
	{
		std::cout << "ouuups no client in map for the fd: " << client_fd << std::endl;
		exit (1);
	}
	Client * client = clients[client_fd];
	// if (events[idx].events & EPOLLHUP)
	// 	std::cout << "*** epoll hang up flag" << std::endl;
	// if (events[idx].events & EPOLLERR)
	// 	std::cout << "*** epoll error  flag" << std::endl;
	memset(recvline, 0, MAXLINE);
	if (events[idx].events & EPOLLIN)
		n = recv(client_fd, recvline, MAXLINE - 1, MSG_DONTWAIT);
	else
	{
		if (!client->check_timeout())
			epoll_remove_client(epollfd, events, clients, client_fd);
		return ;
	}

	std::cout << "*** return of recv: " << n << std::endl;

	if (n == 0)
		epoll_remove_client(epollfd, events, clients, client_fd);

	if (n > 0)
	{
		client->addToMessage((char *)recvline);
		memset(recvline, 0, MAXLINE);
		if (n == MAXLINE && client->getMessage().find("\r\n\r\n") == std::string::npos)
			peek = recv(client_fd, recvline, MAXLINE, MSG_PEEK | MSG_DONTWAIT);
	}
	std::cout << "+++++ FD: "<< events[idx].data.fd << " , FLAG: "<< events[idx].events << std::endl;
	if (n < 0 || peek < 0)
		exit_me("read error");
	if (n <= MAXLINE && client->getMessage().find("\r\n\r\n") != std::string::npos)
	{
		std::cout << std::endl << client->getMessage() << std::endl;
		if (events[idx].events & EPOLLOUT)
		{
			write(client_fd, answer.c_str(), answer.size());
			epoll_remove_client(epollfd, events, clients, client_fd);
			delete client;
		}
	}
}
void	run_poll()
{
	struct epoll_event	events[MAX_EVENTS];
	std::map<int, Client *>	clients;
	int	listen_socket;
	int	nfds;
	int	epollfd;

	listen_socket = init_socket();
	epollfd = init_epoll(listen_socket);
	int i = 1;
	while (true)
	{
		std::cout << "going throught loop " << i << " times" << std::endl;
		nfds = epoll_wait(epollfd, events, MAX_EVENTS, MAX_WAIT);
		if (nfds == -1)
			exit_me("epoll_wait error");
	
		for (int n = 0; n < nfds; ++n)
		{
			if (events[n].data.fd == listen_socket)
				epoll_add_client(epollfd, listen_socket, clients);
			else
				handle_client(epollfd, events[n].data.fd, events, clients, n);
		}
		i++;
		std::cout << "size of map: " << clients.size() << std::endl;
	}
}

int main(void)
{
	run_poll();
}
