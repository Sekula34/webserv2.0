#include "./Parsing/ServersInfo.hpp"
#include "Server/ConnectionManager.hpp"
#include "Message/Node.hpp"
#include "Message/Message.hpp"
#include "Io/Io.hpp"
#include "Server/Socket.hpp"
#include "Utils/Data.hpp"
#include "Utils/FileUtils.hpp"
#include "Utils/Logger.hpp"
#include <csignal>
#include <exception>
#include <stdexcept>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>
// #include <map>
// #include <list>
//

volatile sig_atomic_t flag = 0 ;

void handle_sigint(int sig)
{
	flag = 1;
	(void) sig;
	Logger::warning("CTRL + C caught, Server is starting shutdown procedure ...", "");
}

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

static void	debugFakeVirtualServer()
{
	std::map<int, Client*>::iterator it = Client::clients.begin();
	for (; it != Client::clients.end(); ++it)
	{
		if(it->second->getMsg(Client::REQ_MSG)->getState() == COMPLETE
			&& it->second->getClientState() != Client::DELETEME)
	  		it->second->setClientState(Client::DO_RESPONSE);	
	}
}

bool	shutdown()
{
	if (flag)
	{
		std::map<int, Client*>::iterator it = Client::clients.begin();
		for (; it != Client::clients.end(); ++it)
			it->second->setClientState(Client::DELETEME);
		if (Client::clients.size() == 0)
		{
			Logger::info("Shutdown Procedure complete, Goodbye!", "");
			return (true);
		}
	}
	return (false);
}

void	ConnectionDispatcherTest(char** envp, const std::string& configFilePath)
{
	signal(SIGINT, handle_sigint);

	// ConnectionManager* manager = NULL;
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
	Logger::info("EPOLL Fd: ", epollFd);
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
		debugFakeVirtualServer();	
		// virtualServer.virtualServerLoop();
		// cgi.cgiLoop();
		if (shutdown())
			break;
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
