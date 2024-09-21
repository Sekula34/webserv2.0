#include "./Parsing/ServersInfo.hpp"
#include "Server/ConnectionManager.hpp"
#include "Server/Socket.hpp"
#include "Utils/Data.hpp"
#include "Utils/FileUtils.hpp"
#include "Utils/Logger.hpp"
#include <exception>
#include <stdexcept>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
#include <list>

void	initVars(char** envp, const std::string& configFilePath)
{
	Data::setAllCgiLang();
	ServersInfo serverInfo(configFilePath);
	Logger::info("SERVER IS TURNED ON", "");
	Data::setEnvp(envp);
	// SocketManager sockets(serverInfo.getUniquePorts());

	// Save all sockets Fds
	std::vector<int> serverSockets = serverInfo.getUniquePorts();
	for (std::vector<int>::const_iterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
		Socket currSocket(*it);

	// Initialize epoll
	int epollFd = epoll_create(1);
	if (epollFd == -1)
		throw (std::runtime_error("epoll_create failed"));

	// Create empty clients list and create manager instance
	std::list<Client*> clients;
	ConnectionManager manager(epollFd, clients);
}

void	ConnectionDispatcherTest(char** envp, const std::string& configFilePath)
{
	ConnectionManager* manager;
	initVars(envp, configFilePath);
	// Main Loop
	while (true)
	{
		manager.ConnectionManagerLoop();
		// io.ioLoop();
		// virtualServer.virtualServerLoop();
		// cgi.cgiLoop();
	}
}

std::string getConfigFilePath(int argc, char** argv)
{
	std::string configFilePath = "configuration_files/default.conf";
	if(argc != 1 && argc != 2)
		throw std::runtime_error("Program has to take a configuration file as argument, or use a default path [" + configFilePath + "]");
	if(argc == 2)
		configFilePath = argv[1];
	Logger::info("Config file path is :[" + configFilePath + "]", true);
	return configFilePath;
}

int main(int argc, char** argv, char** envp)
{
	try
	{
		FileUtils::setConfigFilePath(getConfigFilePath(argc, argv));
		//const std::string filePath = getConfigFilePath(argc, argv);
		ConnectionDispatcherTest(envp, FileUtils::getConfigFilePath());
	}
	catch(std::exception &e)
	{
		Logger::error("Exception Happened", true);
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
