#include "HeaderTester.hpp"
#include "../srcs/Client/AHeader.hpp"
#include <cassert>
#include <iostream>


void HeaderTester::runAll()
{
    std::string headers = 
        "Host: localhost:9090\r\n"
        "Connection: keep-alive\r\n"
        "Cache-Control: max-age=0\r\n"
        "sec-ch-ua: \"Not)A;Brand\";v=\"99\", \"Google Chrome\";v=\"127\", \"Chromium\";v=\"127\"\r\n"
        "sec-ch-ua-mobile: ?0\r\n"
        "sec-ch-ua-platform: \"Linux\"\r\n"
        "Upgrade-Insecure-Requests: 1\r\n"
        "User-Agent: Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/127.0.0.0 Safari/537.36\r\n"
        "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,image/apng,*/*;q=0.8,application/signed-exchange;v=b3;q=0.7\r\n"
        "Sec-Fetch-Site: none\r\n"
        "Sec-Fetch-Mode: navigate\r\n"
        "Sec-Fetch-User: ?1\r\n"
        "Sec-Fetch-Dest: document\r\n"
        "Accept-Encoding: gzip, deflate, br, zstd\r\n"
        "Accept-Language: en-US,en;q=0.9,hr;q=0.8,bs;q=0.7\r\n";
	
	AHeader header(headers);
	std::cout << headers << std::endl;
	assert(header.getHeaderFieldMap().size() == 15);
	AHeader empty("");
	assert(empty.getHeaderFieldMap().size() == 0);
	std::cout << empty << std::endl;
	_testpassed();
}
