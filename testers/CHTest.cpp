#include "CHTest.hpp"
#include <cassert>
#include "../srcs/Utils/Logger.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "../srcs/Parsing/ParsingUtils.hpp"


std::vector<std::pair<std::string, int> > CHTest::messages;


void CHTest::runAllTests()
{
	testIfThereIsBody();
	CHBlockTest();
	copyTestBlock();
}


void CHTest::copyTest(const std::string fullMessage)
{
	Logger::testCase("Testing copy construcotr", "to be same");
	ClientHeader first(fullMessage);
	ClientHeader &second(first);
	assert(first.getErrorCode() == second.getErrorCode());
	assert(first.getRequestedUrl() == second.getRequestedUrl());
	_testpassed();
}


void CHTest::compare(const ClientHeader& actual, const std::pair<std::string, int>& expected)
{
	Logger::testCase("Comparing", ". Expected error code is"); 
	std::cout << expected.second <<", actual: " << actual.getErrorCode() << std::endl;
	assert(actual.getFullMessage() == ParsingUtils::extractUntilDelim(expected.first, "\r\n\r\n"));
	assert(actual.getErrorCode() == expected.second);
}

void CHTest::testCHcase(const std::string fullMessage, const std::pair<std::string, int>& expected)
{
	Logger::testCase("Testing simple header", fullMessage);
	ClientHeader header(fullMessage);
	compare(header, expected);
	
	std::cout << header.getFullMessage() << std::endl;
	_testpassed();
}

void CHTest::simplePrint()
{
	Logger::testCase("Simple", "something");
	ClientHeader header(generateValidHttpReques());
	ParsingUtils::printMap(header._headerFields);
}



void CHTest::CHBlockTest()
{
	InitVector();
	for(size_t i = 0; i < messages.size(); i++)
	{
		testCHcase(messages[i].first, messages[i]);
	}
	_testpassed(true);
}


std::string CHTest::generateValidHttpReques()
{
	std::string valid = "GET /path/to/resource HTTP/1.1\r\n\
Host: www.example.com\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n\
Accept-Encoding: gzip, deflate, br\r\n\
Accept-Language: en-US,en;q=0.9\r\n\
Connection: keep-alive\r\n\
\r\n";
	return valid;
}

std::string invalidFirtLineRequest(void)
{
	std::string valid = "GET /path/to/resource HTTP/1.1 \
Host: www.example.com\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n\
Accept-Encoding: gzip, deflate, br\r\n\
Accept-Language: en-US,en;q=0.9\r\n\
Connection: keep-alive\r\n\
\r\n";
	return valid;
}
std::string invalidHttp(void)
{
	std::string valid = "GET /path/to/resource HTTP/2\r\n \
Host: www.example.com\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n\
Accept-Encoding: gzip, deflate, br\r\n\
Accept-Language: en-US,en;q=0.9\r\n\
Connection: keep-alive\r\n\
\r\n";
	return valid;
}

std::string unsuportedMethod(void)
{
	std::string valid = "PUT /path/to/resource HTTP/1.1\r\n\
Host: www.example.com\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n\
Accept-Encoding: gzip, deflate, br\r\n\
Accept-Language: en-US,en;q=0.9\r\n\
Connection: keep-alive\r\n\
\r\n";
	return valid;
}

std::string nornrn(void)
{
	std::string valid = "GET /path/to/resource HTTP/1.1\r\n\
Host: www.example.com\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n\
Accept-Encoding: gzip, deflate, br\r\n\
Accept-Language: en-US,en;q=0.9\r\n\
Connection: keep-alive\r\n";
	return valid;
}


std::string somepartsOfBody()
{
	std::string valid = "GET /path/to/resource HTTP/1.1\r\n\
Host: www.example.com\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.131 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.9\r\n\
Accept-Encoding: gzip, deflate, br\r\n\
Accept-Language: en-US,en;q=0.9\r\n\
Connection: keep-alive\r\n\
\r\nAnd here we have something that belongs to bod.......";
	return valid;
}

std::string validWithContent()
{
	std::string withBody= "POST /path/resource HTTP/1.1\r\n\
Host: www.example.com\r\n\
User-Agent: MyCustomClient/1.0\r\n\
Accept: */*\r\n\
Content-Type: application/json\r\n\
Content-Length: 1234\r\n\
Connection: keep-alive\r\n\
\r\n";

	return withBody;
}
void CHTest::InitVector()
{
	std::pair<std::string, int> pair;
	pair.first = generateValidHttpReques();
	pair.second = 0;
	messages.push_back(pair);

	pair.first = invalidFirtLineRequest();
	pair.second = 400;
	messages.push_back(pair);

	pair.first = invalidHttp();
	pair.second = 505;
	messages.push_back(pair);

	pair.first = unsuportedMethod();
	pair.second = 405;
	messages.push_back(pair);

	pair.first = nornrn();
	pair.second = 400;
	messages.push_back(pair);

	pair.first = somepartsOfBody();
	pair.second = 0;
	messages.push_back(pair);

	pair.first = validWithContent();
	pair.second = 0;
	messages.push_back(pair);

}

void CHTest::copyTestBlock()
{
	copyTest(generateValidHttpReques());
	copyTest(invalidFirtLineRequest());
	_testpassed(true);
}

void CHTest::testIfThereIsBody()
{
	Logger::testCase("Testing if ch expect body", "");
	ClientHeader header(validWithContent());
	assert(header.isBodyExpected() == true);
	ClientHeader head2(generateValidHttpReques());
	assert(head2.isBodyExpected() == false);
	_testpassed();
}
