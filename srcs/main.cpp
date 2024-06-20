#include "./Parsing/ServersInfo.hpp"
#include "Parsing/ServerSettings.hpp"
#include "Server/Socket.hpp"
#include <exception>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>


void socketTest()
{
	Socket firstSocket(8080);
	//int serverSocket = firstSocket.getSocketFd();
	std::cout << firstSocket.getSocketFd() << std::endl;
	int clientSocket = firstSocket.getCommunicationSocket();

	char buffer[1024] = {0};
	int valread = read(clientSocket , buffer, 1024); 
	std::cout << "Valread is " << valread << std::endl;
	std::cout << buffer << std::endl;

}

void serverInfoTest()
{
	ServersInfo servers;

		
	std::vector<ServerSettings> serveri = servers.getAllServers();
	servers.printAllServersInfo();
}

int main()
{
	//

	try
	{
		socketTest();

	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
