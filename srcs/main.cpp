#include "./Parsing/ServersInfo.hpp"
#include "Server/ConnectionDispatcher.hpp"
#include "Server/SocketManager.hpp"
#include "Utils/Data.hpp"
#include "Utils/FileUtils.hpp"
#include "Utils/Logger.hpp"
#include <exception>
#include <iostream>
#include <stdexcept>
#include <sys/socket.h>
#include <unistd.h>
#include <iostream>

void ConnectionDispatcherTest(char** envp, const std::string& configFilePath)
{
	Data::setAllCgiLang();
	ServersInfo serverInfo(configFilePath);
	Logger::info("SERVER IS TURNED ON"); std::cout<<std::endl;
	Data::setEnvp(envp);
	SocketManager sockets(serverInfo.getUniquePorts());
	ConnectionDispatcher dispatcher(sockets, serverInfo);
	dispatcher.mainLoopEpoll();
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
