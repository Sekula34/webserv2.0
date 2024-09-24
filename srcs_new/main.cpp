#include "./Parsing/ServersInfo.hpp"
#include "Client/Client.hpp"
#include "Parsing/ServerSettings.hpp"
#include "Server/ConnectionManager.hpp"
#include "Io/Io.hpp"
#include "Server/Socket.hpp"
#include "Server/VirtualServer.hpp"
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
// #include <map>
// #include <list>

// START Testing
#include "Server/Delete/DummyClient.hpp"
#include "Server/Delete/DummyMessage.hpp"
// END Testing

// void	initVars(char** envp, const std::string& configFilePath)
// {
// 	Data::setAllCgiLang();
// 	ServersInfo serverInfo(configFilePath);
// 	Logger::info("SERVER IS TURNED ON", "");
// 	Data::setEnvp(envp);
// 	// SocketManager sockets(serverInfo.getUniquePorts());

// 	// Save all sockets Fds
// 	std::vector<int> serverSockets = serverInfo.getUniquePorts();
// 	for (std::vector<int>::const_iterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
// 		Socket currSocket(*it);

// 	// Initialize epoll
// 	int epollFd = epoll_create(1);
// 	if (epollFd == -1)
// 		throw (std::runtime_error("epoll_create failed"));

// 	// Create manager instance
// 	ConnectionManager manager(epollFd);
// }


/* void assingServer(Client& client, ServersInfo& servers)
{
	///client get port 
	const ServerSettings* clientServer = servers.getServerByPort(8080);
	client.setServer(clientServer);
} */

void	ConnectionDispatcherTest(char** envp, const std::string& configFilePath)
{
	// ConnectionManager* manager = NULL;
	Data::setAllCgiLang();
	ServersInfo serverInfo(configFilePath);
	Logger::info("SERVER IS TURNED ON", "");
	Data::setEnvp(envp);
	// START of checking
	ServerSettings virtualServer01 = serverInfo.getAllServers()[0];
	// virtualServer01.printServerSettings();
	VirtualServer VS1(virtualServer01);
	DummyMessage* message = new DummyMessage("test", 0);
	DummyClient client(4, 7777, 4);
	client.setRequestMsg(message);
	std::cout << *client.getMsg(DummyClient::REQ_MSG) << std::endl;
	VS1.generateResponse(client);
	delete message;
	// std::cout << serverInfo.getAllServers().size() << std::endl;
	// serverInfo.printAllServersInfo();	
	// END of checking
	// SocketManager sockets(serverInfo.getUniquePorts());

	/* // Save all sockets Fds
	std::vector<int> serverSockets = serverInfo.getUniquePorts();
	for (std::vector<int>::const_iterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
		Socket currSocket(*it);

	// Initialize epoll
	int epollFd = epoll_create(1);
	if (epollFd == -1)
		throw (std::runtime_error("epoll_create failed"));

	// Create manager instance
	ConnectionManager manager(epollFd);
	Io io;
	Logger::info("my pid is: ", getpid());
	// initVars(envp, configFilePath);
	// Main Loop
	while (true)
	{
		manager.epollLoop();
		io.ioLoop();
		// io.ioLoop();
		// virtualServer.virtualServerLoop();
		// cgi.cgiLoop();
	} */
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
