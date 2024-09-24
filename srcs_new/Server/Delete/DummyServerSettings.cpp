#include "DummyServerSettings.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

//==========================================================================//
// REGULAR METHODS==========================================================//
//==========================================================================//

// Method to parse the file and store it in a map
void	DummyServerSettings::loadFile(const std::string& serverSettings)
{
	if (serverSettings.empty())
	{
		std::cout << "serverSettings file is empty" << std::endl;
		return ;
	}
	std::ifstream file(serverSettings.c_str());
	if (!file.is_open())
	{
		std::cerr << "Unable to open config file!" << std::endl;
		return ;
	}
	std::string line;
	while (std::getline(file, line))
	{
		std::string key;
		std::string value;
		std::istringstream lineStream(line);
		if (std::getline(lineStream, key, '=') && std::getline(lineStream, value))
			_mappedFile[key] = value;
	}
	file.close();
	return ;
}

// Function to retrieve value by key
std::string		DummyServerSettings::getValue(const std::string& key) const
{
	std::map<std::string, std::string>::const_iterator it = _mappedFile.find(key);
	if (it != _mappedFile.end()) {
		return it->second;
	}
	return "";
}

// void		DummyServerSettings::printAllValues() const
// {
// 	std::cout << std::endl;
// 	std::cout << "SERVER SETTINGS" << std::endl;
// 	std::map<std::string, std::string>::const_iterator it = _mappedFile.begin();
// 	for (; it != _mappedFile.end(); ++it)
// 		std::cout << it->first << "=" << it->second << std::endl;
// }

//==========================================================================//
// Constructor, Destructor and OCF Parts ===================================//
//==========================================================================//

// Custom Constructor
DummyServerSettings::DummyServerSettings(std::string type)
{
	if (type != "test")
		return ;
	this->loadFile(CONFIGFILE);
}

// Constructor
DummyServerSettings::DummyServerSettings() {}

// Overloaded Insertion Operator (For testing).
std::ostream&	operator<<(std::ostream& out, const DummyServerSettings& serverSettings)
{
	out << "SERVER SETTINGS" << std::endl;
	std::map<std::string, std::string>::const_iterator it = serverSettings._mappedFile.begin();
	for (; it != serverSettings._mappedFile.end(); ++it)
		out << it->first << "=" << it->second << std::endl;
	return (out);
}

// std::cout << "-----------------DEBUGGING-----------------" << std::endl;
