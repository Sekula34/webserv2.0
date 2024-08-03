#ifndef SERVERTEST_HPP
# define SERVERTEST_HPP
#include "UnitTest.hpp"
#include "../srcs/Parsing/ServerSettings.hpp"
#include <cstddef>
#include <vector>

class ServerTest : public UnitTest
{
	private : 
		static void _portTester(int expectedPort, const ServerSettings& server);
		static void _locationNumberTester(size_t expectedNumberOfLocations, const ServerSettings& server);
		static void _locationUriTester(const std::vector<std::string >expectedLocations, const ServerSettings& server);
	public :
		static void amIlocationtest(std::string location, const ServerSettings& server, bool expected);
		static void serverTestCase();
		static void runAllTests();
};

#endif
