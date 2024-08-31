# include <iostream>
# include <sstream>
# include "Message.hpp"
# include "Node.hpp"
# include <fcntl.h>
# include <unistd.h>

int main()
{
	int fd = open("message", O_RDONLY);
	char	buffer[MAXLINE];
	class Message m;


	read(fd, buffer, MAXLINE - 1);

	m.addStr(buffer);




	// 	std::string s = "what's going on this is crazy chunky!";
	// 	class Node node(s, CHUNK);
	// 	std::cout << "unchunked version:" << std::endl << s << std::endl;
	// 	std::cout << "chunked version:" << std::endl << node.getStringChunked();
	//
	// std::cout << std::endl << "================================" << std::endl <<  std::endl;
	//
	// 	class Node node_chunked(node.getStringChunked(), CHUNK);
	// 	std::cout << "chunked version:" << std::endl << node.getStringChunked() << std::endl;
	// 	std::cout << "unchunked version:" << std::endl << node_chunked.getStringUnchunked() << std::endl;
}


