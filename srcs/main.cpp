#include "./Parsing/ServersInfo.hpp"
#include "Client/ClientHeader.hpp"
#include "Client/ClientHeaderManager.hpp"
#include "Client/ClientRequest.hpp"
#include "Parsing/ParsingUtils.hpp"
#include "Parsing/ServerSettings.hpp"
#include "Server/ConnectionDispatcher.hpp"
#include "Server/Socket.hpp"
#include "Server/SocketManager.hpp"

#include <exception>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>





void ClientHeaderManagerTester()
{
	Socket mySocket(8080);
	ClientHeaderManager cm;
	std::cout << "Created ClientHeaderManager " << std::endl;
	int fd = mySocket.getCommunicationSocket();
	//int readfd(4);
	//cm.createNewClientHeader(fd);
	//ClientHeader header(fd);
	//ClientHeader& header = cm.getClientHeader(3);
	ReadStatus status = cm.readClientHeader(fd);
	std::cout <<"Status is " << status << std::endl;
	cm.readClientHeader(fd);
	//cm.readClientHeader(fd);
	//std::cout <<"Status is " << status << std::endl;

}

void clientMessageTest()
{
	Socket mySocket(8080);
	int clientFD = mySocket.getCommunicationSocket();
	ClientHeader message(clientFD);
	std::cout << message << std::endl;
	message.readOnce();
	message.readOnce();
	message.readOnce();
	std::cout << message << std::endl;
	close(clientFD);
	close(mySocket.getSocketFd());
}


void clientRequestTest()
{
	std::string plainValue = ParsingUtils::getHttpPlainValue(" localhost:8080 ");
	//std::cout << "plain value is [" << plainValue <<"]" << std::endl; 
	ClientRequest curlRequest
		("GET / HTTP/1.1\r\n"
		"Host: localhost:8080\r\n"
		"User-Agent: curl/7.XX.X\r\n"
		"Accept: */*\r\n"
		"\r\n");
	std::cout << curlRequest << std::endl;
}


void socketTest()
{
	Socket firstSocket(8080);
	std::cout << firstSocket << std::endl;
	//int serverSocket = firstSocket.getSocketFd();
	std::cout << firstSocket.getSocketFd() << std::endl;
	while(true)
	{
		int clientSocket = firstSocket.getCommunicationSocket();
		char buffer[1024] = {0};
		int valread = read(clientSocket , buffer, 1024); 
		std::cout << "Valread is " << valread << std::endl;
		std::cout << "CLIENT  REQUEST" << std::endl;
		std::cout << buffer << std::endl;
		std::cout <<"END OF CLIENT REQUEST" << std::endl;
		close(clientSocket);
	}

}

void multipleSocketTesting()
{
	ServersInfo servers;
	std::vector<int> uniquePorts = servers.getUniquePorts();
	SocketManager sockets(uniquePorts);
	std::vector<Socket> allSockets = sockets.getAllSockets();
	int clientSocket;

	clientSocket = allSockets[0].getCommunicationSocket();
	char buffer[1024] = {0};
	int valread = read(clientSocket , buffer, 1024); 
	std::cout << "Valread is " << valread << std::endl;
	std::cout << "CLIENT  REQUEST" << std::endl;
	std::cout << buffer << std::endl;
	std::cout <<"END OF CLIENT REQUEST" << std::endl;
	const char *http_response = 
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: 46\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<html><body><h1>Hello, World!</h1></body></html>";
	//const char *hello = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello world!";//IMPORTANT! WE WILL GET TO IT
	write(clientSocket , http_response , strlen(http_response));
	close(clientSocket);
	int clientSocket2 = allSockets[1].getCommunicationSocket();
	char buffer2[1024] = {0};
	valread = read(clientSocket2 , buffer2, 1024); 
	std::cout << "Valread is " << valread << std::endl;
	std::cout << "CLIENT  REQUEST" << std::endl;
	std::cout << buffer2 << std::endl;
	std::cout <<"END OF CLIENT REQUEST" << std::endl;
	close(clientSocket2);
}

void ConnectionDispatcherTest()
{
	ServersInfo serverInfo;
	SocketManager sockets(serverInfo.getUniquePorts());
	ConnectionDispatcher dispatcher(sockets, serverInfo);
	dispatcher.mainLoop();
}

void serverInfoTest()
{
	ServersInfo servers;
	ParsingUtils::printVector(servers.getUniquePorts());
	//servers.getUniquePorts();

		
	std::vector<ServerSettings> serveri = servers.getAllServers();
	servers.printAllServersInfo();
}
void SocketManagerTest()
{
	ServersInfo servers;
	std::vector<int> uniquePorts = servers.getUniquePorts();
	SocketManager sockets(uniquePorts);
	ParsingUtils::printVector(sockets.getAllListenFd());
	std:: cout << sockets.getMaxSocketFd() << std::endl;
}

int main()
{
	//

	try
	{
		//serverInfoTest();
	//	socketTest();
		//multipleSocketTesting();
		ConnectionDispatcherTest();
		//SocketManagerTest();
		//clientRequestTest();
		//clientMessageTest();
		//ClientHeaderManagerTester();
	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
