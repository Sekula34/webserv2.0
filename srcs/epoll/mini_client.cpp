

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
#include <iostream>

#define SERVER_PORT	18000

#define MAXLINE		4096
#define SA struct sockaddr

void	exit_me(std::string s)
{
	std::cout << s << std::endl;
	exit (1);
}


int main(int argc, char **argv)
{
	int					sockfd, n;
	int					sendbytes;
	struct sockaddr_in	servaddr;
	char				sendline[MAXLINE];
	char				recvline[MAXLINE];


	if (argc != 2)
		exit_me("need exactly two arguments");

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		exit_me("Error while creating the socket!");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERVER_PORT);

	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0)
		exit_me("inet error");

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0)
		exit_me("connect failed");

	// sprintf(sendline, "GET / HTTP/1.1 ... noice\r\n\r\n");
	sprintf(sendline, "");
	sendbytes = strlen(sendline);

	if (write(sockfd, sendline, sendbytes) != sendbytes)
		exit_me("write into socket fialed");

	memset(recvline, 0, MAXLINE);

	while ((n = read(sockfd, recvline, MAXLINE-1)) > 0)
	{
		printf("%s", recvline);
		memset(recvline, 0, MAXLINE);
	}
	if (n < 0)
		exit_me("read from socket fialed");

	exit (0);
}
