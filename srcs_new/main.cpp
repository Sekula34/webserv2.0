#include "Server/ServerManager.hpp"
#include "Server/ConnectionManager.hpp"
#include "Io/Io.hpp"
#include "Cgi/Cgi.hpp"
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
// #include <map>
// #include <list>
//

// void	initVars(char** envp, const std::string& configFilePath)
// {
// 	Data::setAllCgiLang();
// 	ServerManager serverInfo(configFilePath);
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

volatile sig_atomic_t flag = 0 ;

void handle_sigint(int sig)
{
	flag = 1;
	(void) sig;
	Logger::warning("CTRL + C caught, Server is starting shutdown procedure ...", "");
}

// static void	debugFakeVirtualServer()
// {
// 	std::map<int, Client*>::iterator it = Client::clients.begin();
// 	for (; it != Client::clients.end(); ++it)
// 	{
// 		if(it->second->getMsg(Client::REQ_MSG)->getState() == COMPLETE
// 			&& it->second->getClientState() != Client::DELETEME)
// 	  		it->second->setClientState(Client::DO_RESPONSE);	
// 	}
// }

bool	shutdownServer()
{
	// SETS THE STATE OF ALL CLIENTS TO DELETEME
	if (flag == 1)
	{
		std::map<int, Client*>::iterator it = Client::clients.begin();
		for (; it != Client::clients.end(); ++it)
			it->second->setClientState(Client::DELETEME);
		flag = 2;
	}

	// IF ALL CLIENTS HAVE BEEN DELETED -> RETURN TRUE TO BREAK OUT OF MAIN LOOP
	if (flag == 2 && Client::clients.size() == 0)
	{
		Logger::info("Shutdown Procedure complete, Goodbye!", "");
		return (true);
	}
	return (false);
}

void	ConnectionDispatcherTest(char** envp, const std::string& configFilePath)
{
	signal(SIGINT, handle_sigint);

	// ConnectionManager* manager = NULL;
	Data::setAllCgiLang();
	Data::setEnvp(envp);
	ServerManager serverInfo(configFilePath);
	Logger::info("SERVER IS TURNED ON", "");
	Data::setEnvp(envp);
	// SocketManager sockets(serverInfo.getUniquePorts());
	
	// Initialize epoll
	int epollFd = epoll_create(1);
	Logger::info("EPOLL Fd: ", epollFd);
	if (epollFd == -1)
		throw (std::runtime_error("epoll_create failed"));

	// Save all sockets Fds
	std::vector<int> serverSockets = serverInfo.getUniquePorts();
	for (std::vector<int>::const_iterator it = serverSockets.begin(); it != serverSockets.end(); ++it)
		Socket currSocket(*it);


	// Create manager instance
	ConnectionManager manager(epollFd);
	Io io;
	Cgi cgi;
	Logger::info("my pid is: ", getpid());
	// initVars(envp, configFilePath);
	// Main Loop
	while (true)
	{
		manager.epollLoop();
		io.ioLoop();
		serverInfo.loop();
		cgi.loop();
		
		// THIS WILL BE REPLACED BY REAL VIRTUAL SERVER FUNCTION
		//debugFakeVirtualServer();	

		if (shutdownServer())
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
		std::string configFilePath = getConfigFilePath(argc, argv);
		FileUtils::setConfigFilePath(configFilePath);
		//const std::string filePath = getConfigFilePath(argc, argv);
		ConnectionDispatcherTest(envp, configFilePath);
	}
	catch(std::exception& e)
	{
		Logger::error("Exception Happened", true);
		std::cerr << e.what() << std::endl;
		return 1;
	}
	return 0;
}
