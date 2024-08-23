#include <sys/socket.h>
#include <sys/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/epoll.h>
#include <iostream>
#include <map>
#include <string>
#include "Client.hpp"


#define SERVER_PORT		18000
// #define MAXLINE			4096
// #define MAXLINE			493
#define MAXLINE			50
#define MAX_EVENTS		10
#define MAX_LISTEN		10
#define MAX_WAIT		-1 // -1 will block forever, 0 not at all and rest is ms
#define MAX_TIMEOUT		10000

#define SA struct sockaddr

class Client;

Client*	find_client_in_clients(int client_fd, std::map<int, Client *> & clients);
bool	read_client(struct epoll_event* events, std::map<int, Client *> & clients, Client * client, int* n, int idx);
bool	read_header(struct epoll_event* events, std::map<int, Client *> & clients, Client* client,  int idx);
void	epoll_add_client(int epollfd, int listen_socket, std::map<int, Client *>& clients);
void	exit_me(std::string s);
void	epoll_remove_client(struct epoll_event* events, std::map<int, Client*> & clients, Client* client);
void	write_client(struct epoll_event* events, std::map<int, Client *> & clients, Client* client,  int idx);
void	handle_client(struct epoll_event* events, std::map<int, Client *> & clients, int idx);
void	run_poll();
int		init_epoll(int listen_sock);
int		init_socket();
