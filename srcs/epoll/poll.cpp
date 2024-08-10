# include "poll.hpp"

// FAKE EXIT FUNCTION, NEEDS TO BE REPLACED BY REAL ONE
void	exit_me(std::string s)
{
	std::cout << s << std::endl;
	exit (EXIT_FAILURE);
}

int	init_epoll(int listen_sock)
{
	// STRUCT NEEDED FOR EPOLL TO SAVE FLAGS INTO (SETTINGS)
	struct epoll_event	ev; 

	// CREATING EPOLL
	int epollfd = epoll_create(1);
	if (epollfd == -1)					
		exit_me("epoll_create error");

	// SETTING UP EV EVENTS 'SETTINGS' STRUCT FOR LISTEN_SOCKET
	ev.events = EPOLLIN | EPOLLOUT;				
	ev.data.fd = listen_sock;

	// ADDING LISTEN_SOCKET TO EPOLL WITH THE EV 'SETTINGS' STRUCT
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) 
		exit_me("epoll_ctl_add - listen socket error");
	return (epollfd);
}

// CREATE LISTEN SOCKET, BIND IT AND LISTEN TO IT
int	init_socket()
{
	struct sockaddr_in	servaddr;
	int	listen_socket;
	int opt(1);

	if ((listen_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit_me("socket error");
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERVER_PORT);

	// PREVENTS BIND ERR WHEN SOCKET IS QUICKLY REUSED
    if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
				   &opt, sizeof(opt)) == -1)
		exit_me("setsockopt error");

	// BIND
	if ((bind(listen_socket, (SA *) &servaddr, sizeof(servaddr))) < 0)
		exit_me("bind error");

	// LISTEN
	if ((listen(listen_socket, MAX_LISTEN)) < 0)
		exit_me("listen error");
	return (listen_socket);
}

// CREATE CLIENT FD BY CALLING ACCEPT ON LISTEN SOCKET, CREATE CLIENT INSTANCE
// ADD INSTANCE TO CLIENTS MAP. MAP KEY: CLIENT FD, MAP VALUE CLIENT INSTANCE POINTER
void	epoll_add_client(int epollfd, int listen_socket, std::map<int, Client *>& clients)
{
	struct epoll_event	ev;
	int	client_fd;

	// ACCEPT RETURNS CLIENT FD
	client_fd = accept(listen_socket, (SA *) NULL, NULL);
	if (client_fd == -1)
		exit_me("accept error");

	// CREATE NEW CLIENT INSTANCE WITH CLIENT FD CONSTRUCTOR
	Client * newClient = new Client(client_fd, epollfd);

	// IF CLIENT FD ALREADY EXISTS IN MAP, THEN SET NOWRITE IN THE CLIENT INSTANCE.
	// USUALLY THE MAP SHOULD NOT CONTAIN A KEY VALUE THAT
	// EQUALS CLIENT FD. THIS CAN ONLY HAPPEN WHEN THERE IS A READ ERROR
	// OR CLIENT HAS CLOSED THE CONNECTION BEFORE A WRITE COULD HAPPEN
	// IN THAT CASE THE  CLIENT INSTANCE NEEDS TO FINISH ITS JOB
	// AND HAS TO BE DELETED -> THIS CASE IS NOT YET TAKEN CARE OF
	if (clients.find(client_fd) != clients.end())
		clients[client_fd]->setNoWrite();
	clients[client_fd] = newClient;

	// ADD CLIENT FD TO THE LIST OF FDS THAT EPOLL IS WATCHING FOR ACTIVITY
	ev.events = EPOLLIN | EPOLLOUT;
	ev.data.fd = client_fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, client_fd, &ev) == -1)
		exit_me("epoll_ctl - conn_socket error");
}

void	epoll_remove_client(struct epoll_event* events, std::map<int, Client*> & clients, Client* client)
{
	// WRITING TO CLIENT FD IS FROM NOW ON FORBIDDEN FOR THIS CLIENT INSTANCE
	clients[client->getFd()]->setNoWrite();
	
	// REMOVE THIS CLIENT INSTANCE FROM CLIENTS MAP
	clients.erase(client->getFd());

	// REMOVE THE FD OF THIS CLIENT INSTANCE FROM EPOLLS WATCH LIST
	epoll_ctl(client->getEpollFd(), EPOLL_CTL_DEL, client->getFd(), events);

	// CLOSE FD
	close (client->getFd());
}

Client*	find_client_in_clients(int client_fd, std::map<int, Client *> & clients)
{
	std::map<int, Client*>::iterator it = clients.find(client_fd);
	if (it == clients.end())
	{
		std::cout << "ouuups no client with fd: " << client_fd
			<< " can be found in clients map! FATAL ERROR!"<< std::endl;
		exit (EXIT_FAILURE);
	}
	return (it->second);
}

bool	read_client(struct epoll_event* events, std::map<int, Client *> & clients, Client * client, int* n, int idx)
{

	if (events[idx].events & EPOLLIN)
		*n = recv(client->getFd(), client->getRecvLine(), MAXLINE - 1, MSG_DONTWAIT);
	else
	{
		if (!client->check_timeout())
			epoll_remove_client(events, clients, client);
		return (false);
	}
	return (true);
}

void	handle_client(int epollfd, int client_fd, struct epoll_event* events,
				   std::map<int, Client *> & clients, int idx)
{
	std::string	answer = "HTTP/1.0 200 OK\r\n\r\nWebserv 0.0";
	int			n = 0;
	int			peek = 0;


	// CHECK WHETHER CLIENT FD CAN BE FOUND IN CLIENTS MAP AND RETURN CLIENT POINTER
	Client * client = find_client_in_clients(client_fd, clients);

	// CHECK IF WE ARE ALLOWED TO READ FROM CLIENT. IF YES READ, IF NO -> RETURN
	// ALSO REMOVES CLIENT ON TIMEOUT
	if (!read_client(events, clients, client, &n, idx))
		return ;

	// CLIENT HAS CLOSED CONNECTION, WE REMOVE CLIENT FROM CLIENTS AND FROM EPOLL
	// RIGHT NOW MEMORY LEAK!!!
	if (n == 0)
		epoll_remove_client(events, clients, client);

	// SUCCESSFUL RECIEVE -> ADDING BUFFER FILLED BY RECIEVE TO THE MESSAGE STRING
	if (n > 0)
	{
		client->addRecvLineToMessage();
		if (n == MAXLINE && client->getMessage().find("\r\n\r\n") == std::string::npos)
			peek = recv(client_fd, client->getRecvLine(), MAXLINE, MSG_PEEK | MSG_DONTWAIT);
	}

	// UNSUCCESSFUL RECIEVE -> REMOVE CLIENT FROM CLIENTS AND EPOLL. LOG ERR MSG
	// RIGHT NOW MEMORY LEAK!!!
	if (n < 0 || peek < 0)
	{
		epoll_remove_client(events, clients, client);
		std::cout << "error: recieve" << std::endl;
	}

	if (n <= MAXLINE && client->getMessage().find("\r\n\r\n") != std::string::npos)
	{
		if (events[idx].events & EPOLLOUT)
		{
			std::cout << std::endl << client->getMessage() << std::endl;
			write(client_fd, answer.c_str(), answer.size());
			epoll_remove_client(events, clients, client);
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
	std::cout << "server listening on port: " << SERVER_PORT << std::endl;
	while (true)
	{
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
		std::cout << "size of map: " << clients.size() << std::endl;
	}
}

int main(void)
{
	run_poll();
}
