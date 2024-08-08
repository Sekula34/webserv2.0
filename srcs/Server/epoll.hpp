# pragma once

#include <map>
#include <string>
#include <iostream>
#include <sys/epoll.h>
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

#define SERVER_PORT		18000
// #define MAXLINE			4096
// #define MAXLINE			493
#define MAXLINE			50
#define MAX_EVENTS		10
#define MAX_LISTEN		10
#define MAX_WAIT		-1 // -1 will block forever, 0 not at all and rest is ms

#define SA struct sockaddr

void	exit_me(std::string s);

void	epoll_add_client(int epollfd, int listen_socket);
void	handle_client(int epollfd, int connfd, struct epoll_event* events,
				   std::map<int, std::string> & messages, int idx);
void    epoll_add_listen_sock(int listen_sock, int epollfd);
int     epoll_loop(void);
int	    init_epoll(void);
int	    init_socket();