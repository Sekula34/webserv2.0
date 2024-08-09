#pragma once 
#include "../srcs/Client/ClientRequestHeader.hpp"
#include "UnitTest.hpp"
#include <string>
#include <utility>
#include <vector>



class CHTest : UnitTest
{
	public:
		static std::vector<std::pair<std::string, int> >messages;

		static void InitVector();
		static void testCHcase(const std::string fullMessage, const std::pair<std::string, int>& expected);
		static void CHBlockTest();
		static void compare(const ClientRequestHeader& actual,const std::pair<std::string, int>& expected);
		static std::string generateValidHttpReques();
	private:
		//static const client

};
