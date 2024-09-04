# include <iostream>
# include <sstream>
# include "Message.hpp"
# include "Node.hpp"
# include <fcntl.h>
# include <unistd.h>
# include <string.h>

int main()
{
	char			buffer[MAXLINE];
	class Message	m;
	size_t			num;


	int fd = open("chunked_request", O_RDONLY);
	// int fd = open("request", O_RDONLY);
	while (true)
	{
		memset(buffer, 0, MAXLINE);
		if ((num = read(fd, buffer, MAXLINE)) <= 0)
			break;
		m.bufferToNodes(buffer, num);
		if (m.getState() == COMPLETE || m.getState() == ERROR)
			break;
	}
	if (m.getState() == ERROR)
		std::cout << "Invalid Request from Client!" << std::endl;
	else
		m.printChain();
	close (fd);

	




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


