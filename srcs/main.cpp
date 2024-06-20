#include "./Parsing/ServersInfo.hpp"
#include "Parsing/ServerSettings.hpp"
#include <exception>
#include <iostream>

int main()
{

	try
	{
		ServersInfo servers;

		
		std::vector<ServerSettings> serveri = servers.getAllServers();
		servers.printAllServersInfo();

	}
	catch(std::exception &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
