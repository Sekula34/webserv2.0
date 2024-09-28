#ifndef SERVERTEST_HPP
# define SERVERTEST_HPP
#include "UnitTest.hpp"
#include "../srcs_new/Server/VirtualServer.hpp"
#include <cstddef>
#include <vector>

class ServerTest : public UnitTest
{
	private : 
		static void _portTester(int expectedPort, const VirtualServer& server);
		static void _locationNumberTester(size_t expectedNumberOfLocations, const VirtualServer& server);
		static void _locationUriTester(const std::vector<std::string >expectedLocations, const VirtualServer& server);
		static void _amIlocationtest(std::string location, const VirtualServer& server, bool expected);
		static void _locationFromUrlGetterTest(const VirtualServer& server, std::string url, std::string expectedResult);
		static void _locationFromUrlBlock(const VirtualServer& server);
	public :
		static void serverTestCase();
		static void runAllTests();
};

#endif
