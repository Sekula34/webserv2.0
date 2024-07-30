#include <cstddef>
#include <exception>
#include <iostream>
#include <vector>
#include "../srcs/Parsing/Configuration.hpp"
#include "../srcs/Parsing/Token.hpp"
#include "../srcs/Parsing/DefaultSettings.hpp"
#include "../srcs/Parsing/ServerSettings.hpp"
#include "../srcs/Parsing/LocationSettings.hpp"
#include "../srcs/Parsing/Directive.hpp"
#include "../srcs/Parsing/ParsingUtils.hpp"
#include "../srcs/Parsing/NginxReturn.hpp"
#include "../srcs/Parsing/ServersInfo.hpp"
    // ANSI escape code for red text
const std::string red("\033[31m");
const std::string green("\033[32m");
    // ANSI escape code to reset color
const std::string reset("\033[0m");



void utilsTesting()
{
	DefaultSettings b;
	std::vector<std::string> hej;
	hej.push_back("jedan");
	hej.push_back("dva");
	b.setIndexes(hej);
	b.printAllSettings();
	NginnxReturn a(1,200, "bokte nece");
	//a.printNginxReturnInfo();
	size_t value = ParsingUtils::stringToSizeT("18446744073709551616");
	std::cout << "Value is " << value << std::endl;

}

void tokenTesting()
{
	std::vector<Token> path;
	Token hej("listen :80", 2);
	Directive directiva(hej);
	directiva.printDirectiveInfor();
	// path.push_back(hej);
	// Token first("listen 8080;");
	// first.setTokenPath(path);
	//hej._checkOneToken();
	//hej.printTokenInfo();
	//first.setTokenType(CONTEXT);
	//first.printTokenInfo();
}

void defSettingsTesting()
{
	DefaultSettings setings;
	//setings.printAllSettings();
	setings.setErrorPage(302, "noviPath");
	std::cout << "New error pages" << std::endl;
	//setings.printAllSettings();
	// setings.changeErrorPage("New path");
	// //setings.printAllSettings();
	// //ServerSettigns server(2, setings);
	// //setings.printAllSettings();
	// server.changeErrorPage("ServerPath");
	Configuration conf("Configuration/config_files/simple_nginx_conf.conf");
	std::vector<Token> allTokens = conf.getAllTokens();
	ServerSettings server(1, setings, allTokens);
	server.printAllSettings();
	// //server2.printAllSettings();
	// //server.printAllSettings();

	// location.printAllSettings();
	// ServerSettigns server;
	// server.printServerSettings();
	// LocationSettings location;

}

void locationTesting()
{
	DefaultSettings settings;
	Configuration conf("Configuration/config_files/simple_nginx_conf.conf");
	std::vector<Token> allTokens = conf.getAllTokens();
	ServerSettings server(1, settings, allTokens);
	//Directive::printAllDirectives(server.getServerDirectives());
	//server.printServerTokens();
	std::vector<Token> serverTokens = server.getServerTokens();
	for(size_t i = 0; i < serverTokens.size(); i++)
	{
		if(serverTokens[i].getCurrentTokenContextType() == LOCATION)
		{
			//LocationSettings location(server, serverTokens[i]);
			LocationSettings location(server, serverTokens[i], serverTokens);
		}
	}
}
void configTesting()
{
	Configuration conf("Configuration/config_files/simple_nginx_conf.conf");

	std::vector<Token> tokens = conf.getAllTokens();
	int numberOfServers = conf.getNumberOfServers();
	std::cout << "There is "  << numberOfServers << " servers in config file" << std::endl;

	DefaultSettings settings;
	//settings.setNginxReturn(303, "aj tu idi");
	ServerSettings server(1, settings, tokens);
	//settings.printAllSettings();
	//std::vector<Directive> serverDir = server.getServerDirectives();
	//serverDir[2].printDirectiveInfor();
	//serverDir[2].apply(server);
//	server.printAllSettings();
	std::vector<LocationSettings> locations = server.getServerLocations();
	LocationSettings oneLocation = locations[0];
	//oneLocation.printLocationSettings();
	std::vector<Directive> locationDirectives = oneLocation.getLocationDirectives();
	Directive dir = locationDirectives[0];
	//dir.printDirectiveInfor();
	dir.apply(oneLocation);
	oneLocation.printLocationSettings();
	// server.printServerSettings();
}

void allInfoTest()
{
	ServersInfo allInfo;
	//allInfo.printAllServersInfo();
	 ServerSettings oneServer =allInfo.getServerById(0);
	 //oneServer.printServerSettings();
	// std::cout << oneServer.getRoot() << std::endl;
	std::cout << oneServer.getServerName() << std::endl;

}


int main()
{
	try
	{
		allInfoTest();
		std::cout << "Start learning socker programmming" << std::endl;
		//utilsTesting();
		
		//configTesting();

		//locationTesting();
		//tokenTesting();
		//defSettingsTesting();
		// std::cout << "Testing main for config files " << std::endl;
		//Configuration conf("Configuration/config_files/simple_nginx_conf.conf");
		// std::cout <<"Path of conf file is :[" << conf.getFilePath() << "]" << std::endl;
		// std::cout << "Stored file content is " << std::endl;
		//conf.printFileMember();

	}
	catch(std::exception& e)
	{
		std::cerr << red << e.what() << reset << std::endl;
		return 1;
	}
	std::cout << green <<"Success" << reset << std::endl;
	return 0;
}
