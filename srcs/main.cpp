#include "./Parsing/ServersInfo.hpp"
#include "Parsing/ServerSettings.hpp"
#include "Server/Socket.hpp"
#include <exception>
#include <iostream>

void socketTest()
{
	Socket firstSocket(8080);
	std::cout << firstSocket.getSocketFd() << std::endl;
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
