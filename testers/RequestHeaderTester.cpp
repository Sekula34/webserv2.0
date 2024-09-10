#include "RequestHeaderTester.hpp"
#include <cassert>
#include "../srcs/Utils/Logger.hpp"
#include <cstddef>
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include "../srcs/Parsing/ParsingUtils.hpp"
#include "../srcs/Response/ResponseHeader.hpp"

std::string RequestHeaderTester::generateValidHttpReques()
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

std::string generateComplexValid(void)
{
	std::string complex ="GET /cgi-bin/hello.py/something/?name=John HTTP/1.1\r\n\
Host: www.example.com\r\n\
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/92.0.4515.107 Safari/537.36\r\n\
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n\
Accept-Language: en-US,en;q=0.5\r\n\
Accept-Encoding: gzip, deflate, br\r\n\
Connection: keep-alive\r\n\
Upgrade-Insecure-Requests: 1\r\n\r\n";
	return complex;
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
	std::string valid = "GET /path/to/resource HTTP/2\r\n\
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

std::vector<std::pair<std::string, int> > RequestHeaderTester::messages;


void RequestHeaderTester::runAllTests()
{
	testIfThereIsBody();
	CHBlockTest();
	copyTestBlock();
	fullUrlTest();
	cgiResponseHeader();
}

void RequestHeaderTester::fullUrlTest()
{
	Logger::testCase("Testing getting cliend full Url");
	RequestHeader header(generateValidHttpReques());
	assert(header.getFullClientURL() == "http://www.example.com:80/path/to/resource");
	_testpassed();
}

void RequestHeaderTester::uriTest()
{
	Logger::testCase("Testing uri");
	RequestHeader header(generateComplexValid());
	std::string uri = header.getURLSuffix();
	std::cout << "Uri is: [" << uri << "]"<< std::endl;
}

void RequestHeaderTester::copyTest(const std::string fullMessage)
{
	Logger::testCase("Testing copy construcotr", "to be same");
	RequestHeader first(fullMessage);
	RequestHeader &second(first);
	assert(first.getHttpStatusCode() == second.getHttpStatusCode());
	assert(first.getURLSuffix() == second.getURLSuffix());
	_testpassed();
}


void RequestHeaderTester::compare(const RequestHeader& actual, const std::pair<std::string, int>& expected)
{
	Logger::testCase("Comparing", ". Expected error code is"); 
	std::cout << expected.second <<", actual: " << actual.getHttpStatusCode() << std::endl;
	assert(actual.getFullMessage() == ParsingUtils::extractUntilDelim(expected.first, "\r\n\r\n"));
	assert(actual.getHttpStatusCode() == expected.second);
}

void RequestHeaderTester::testCHcase(const std::string fullMessage, const std::pair<std::string, int>& expected)
{
	Logger::testCase("Testing simple header", fullMessage);
	RequestHeader header(fullMessage);
	compare(header, expected);
	
	std::cout << header.getFullMessage() << std::endl;
	_testpassed();
}

void RequestHeaderTester::simplePrint()
{
	Logger::testCase("Simple", "something");
	RequestHeader header(generateValidHttpReques());
	std::cout << header << std::endl;
}



void RequestHeaderTester::CHBlockTest()
{
	InitVector();
	for(size_t i = 0; i < messages.size(); i++)
	{
		std::cout << "Test case " << i << std::endl;
		if(i == 2)
		{
			std::cout << "Debug this" << std::endl;
		}
		testCHcase(messages[i].first, messages[i]);
	}
	_testpassed(true);
}


void RequestHeaderTester::InitVector()
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

	pair.first = generateComplexValid();
	pair.second = 0;
	messages.push_back(pair);

}

void RequestHeaderTester::copyTestBlock()
{
	copyTest(generateValidHttpReques());
	copyTest(invalidFirtLineRequest());
	_testpassed(true);
}

void RequestHeaderTester::testIfThereIsBody()
{
	Logger::testCase("Testing if ch expect body", "");
	RequestHeader header(validWithContent());
	assert(header.isBodyExpected() == true);
	RequestHeader head2(generateValidHttpReques());
	assert(head2.isBodyExpected() == false);
	_testpassed();
}

void RequestHeaderTester::cgiResponseHeader()
{
	std::string cgiResponse = "HTTP/1.1 200 OK\n\
Connection: close\n\
Content-Language: en\n\
Content-Length: 1000\n";

	std::string cgiResponseNoStatus = "Connection: close\n\
Content-Language: en\n\
Content-Length: 1000\n";
	ResponseHeader* cgi = ResponseHeader::createCgiResponseHeader(cgiResponseNoStatus, "\n");
	delete cgi;
	cgi = ResponseHeader::createCgiResponseHeader(cgiResponse, "\n");
	delete cgi;
}
