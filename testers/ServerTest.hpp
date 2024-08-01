#ifndef SERVERTEST_HPP
# define SERVERTEST_HPP
#include "UnitTest.hpp"
#include "../srcs/Parsing/ServerSettings.hpp"

class ServerTest : public UnitTest
{
	private : 
		static void _portTester(int expectedPort, const ServerSettings& server);

	public :
		static void serverTestCase();
		static void runAllTests();
};

#endif
