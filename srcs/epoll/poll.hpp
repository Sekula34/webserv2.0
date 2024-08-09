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
#define MAX_TIMEOUT		1000

#define SA struct sockaddr

int		init_epoll(int listen_sock);
int		init_socket();
void	exit_me(std::string s);
void	epoll_add_client(int epollfd, int listen_socket);
void	handle_client(int epollfd, int connfd, struct epoll_event* events,
				   std::map<int, std::string> & messages, int idx);
void	run_poll();
