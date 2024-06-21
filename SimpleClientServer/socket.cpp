#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <sys/socket.h>
#include <iostream>
#include <unistd.h>
#include <netinet/in.h> //for sokcaddr_in;
#include <arpa/inet.h> //for htonl
#include <string.h> //for memset
#include <iostream>

//converte number to network (which is big endian)
//reverse action form network to host (host is little endian on UBUNTU)
void hostToNetwork()
{
	uint32_t number(8080);
	std::cout << "Number is " << number << std::endl;
	uint32_t converted = htonl(number);
	std::cout <<"Converted number is " << converted << std::endl;
	std::cout << "REverse action is " << ntohl(converted) << std::endl;
	exit(EXIT_SUCCESS);
}

int main()
{
	//hostToNetwork();
	const int port = 8080;
	int serverFd;

	serverFd = socket(AF_INET, SOCK_STREAM, 0);
	if(serverFd < 0)
	{
		perror("Cannot create socket");
		return 1;
	}
	std::cout << "ServerFd is " << serverFd << std::endl;

	struct sockaddr_in adress;
	int addrlen = sizeof(adress);


	memset((char *)&adress, 0, sizeof(adress));
	adress.sin_family = AF_INET;
	adress.sin_addr.s_addr = htonl(INADDR_ANY);
	adress.sin_port = htons(port);

		int opt = 1;
	setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	

	int bindValue = bind(serverFd, (struct sockaddr*)&adress, sizeof(adress));
	if(bindValue != 0)
	{
		perror("Bind failed");
		return 1;
	}
	std::cout << "Bind was successfull " << std::endl;
	int listenValue = listen(serverFd, 3);
	if(listenValue != 0)
	{
		perror("Listen failed");
		return 1;
	}
	std::cout << "Server is litening on port " << port << std::endl;
	while (true)
	{
		int communicationSocket = accept(serverFd, (struct sockaddr *) &adress, (socklen_t *) &addrlen);
		if(communicationSocket < 0)
		{
			perror("Accept failed");
			std::cout << errno << std::endl;
			return 1;
		}
		std::cout << "Accept was succesfull communicaton socket fd is" << communicationSocket << std::endl;

		char buffer[1024] = {0};
		int valread = read( communicationSocket , buffer, 1024); 
		std::cout << buffer << std::endl;
		//printf("%s/n",buffer );
		if(valread < 0)
		{ 
			printf("No bytes are there to read");
		}
		   const char *http_response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 46\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><body><h1>Hello, World!</h1></body></html>";
		const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";//IMPORTANT! WE WILL GET TO IT
		write(communicationSocket , http_response , strlen(http_response));
		close(communicationSocket);
	}


	//bindVal = bind(serverFd, const struct sockaddr *addr, socklen_t len)

	//int opt = 1;
	//setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
	close(serverFd);


	return 0;
}
