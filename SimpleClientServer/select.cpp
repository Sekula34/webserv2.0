#include <cstdio>
#include <fcntl.h>
#include <sstream>
#include <stdexcept>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>

#include <iostream>
#include <unistd.h>


int returnFd(int number)
{
	std::stringstream ss;
	ss<<number;
	std::string fileName = "file" + ss.str() + ".txt";
	int fd = open(fileName.c_str(),O_RDWR | O_CREAT, 0644);
	if(fd == -1)
	{
		perror("open");
		throw std::runtime_error("RIP");
	}
	return(fd);
}

int manExample()
{
	int             retval;
	fd_set          rfds;
	struct timeval  tv;

	/* Watch stdin (fd 0) to see when it has input. */

	FD_ZERO(&rfds);
	FD_SET(0, &rfds);

	/* Wait up to five seconds. */

	tv.tv_sec = 5;
	tv.tv_usec = 0;

	retval = select(1, &rfds, NULL, NULL, &tv);
	/* Don't rely on the value of tv now! */

	if (retval == -1)
		perror("select()");
	else if (retval)
		printf("Data is available now.\n");
		/* FD_ISSET(0, &rfds) will be true. */
	else
		printf("No data within five seconds.\n");

	exit(EXIT_SUCCESS);
 }

int main()
{
	manExample();
	fd_set citaj;
	FD_ZERO(&citaj);
	fd_set pisi;
	FD_ZERO(&pisi);
	for(int i = 0 ; i < 7; i++)
	{	
		int fd = returnFd(i);
		FD_SET(fd, &citaj);
	}
	//write(fd, "Pisi ovo u fajlu\n", 19);
	std::cout << "Pisi i citaj su zerovani" << std::endl;
	std::cout << "Nucit jebeni select vise" << std::endl;
	return 0;
}
