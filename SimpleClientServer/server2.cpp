#include <asm-generic/socket.h>
#include <bits/types/struct_timeval.h>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/select.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>


int nArrClient[5]; //handle only 5 clients


void ProcessNewMessage(int nClientSocket)
{
	std::cout <<"Processing the new message for client socket: " << nClientSocket << std::endl;
	char buff[257] = {0};
	int nRet = recv(nClientSocket, buff, 256,0);
	if(nRet < 0)
	{
		std::cout << "Something wrong happened..closing the connection for client " << std::endl;
		close(nClientSocket);
		for(int i = 0; i < 5; i++)
		{
			if(nArrClient[i] == nClientSocket)
			{
				nArrClient[i] = 0;
				break;
			}
		}
	}
	else 
	{
		std::cout << "The message received from client is : " << buff << std::endl;
		send(nClientSocket, "Proces rikvest",15, 0); 
	}
}

void ProcessTheNewRequest(int& nSocket, fd_set& frREf )
{
	fd_set* fr = &frREf;
	if( FD_ISSET(nSocket, fr))
	{
		int nLen = sizeof(struct sockaddr);
		int nClientSocket = accept(nSocket, NULL, (socklen_t *)&nLen);
		if(nClientSocket > 0)
		{
			//put it inot the client fd_set;
			int i;
			for(i = 0; i < 5; i++)
			{
				if(nArrClient[i] == 0)
				{
					nArrClient[i] = nClientSocket;
					send(nClientSocket, "I got the connection ", 22, 0);
					break;
				}
			}
			if(i == 5)
			{
				std::cout <<"No space for a new connection " << std::endl;
			}
			
		}
		else if(nClientSocket == -1)
		{
			std::cout << "ACCEPT ERROR" << std::endl;
		}
	}
	else
	{
		for(int i = 0; i < 5; i++)
		{
			if(FD_ISSET(nArrClient[i], fr))
			{
				//got the new message from the client
				//juse recv new message 
				//just que that for new workes of your server to fulfill the
				//request
				//PRocessNewMessage(nArrClient[i])
				std::cout << "I am here" << std::endl;
				ProcessNewMessage(nArrClient[i]);
			}
		}
	}
}

int main()
{
	struct sockaddr_in srv;
	fd_set fr, fw, fe;
	const int port(9090);
	int nMaxFd;
	int nRet;

	//initialize the socket
	int nSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(nSocket == -1)
	{
		perror("socket");
		return 1;
	}
	std::cout << "Socket is opened and fd is " << nSocket << std::endl;
	
	//initialize the environment for socket structure
	srv.sin_family = AF_INET;
	srv.sin_port = htons(port);
	srv.sin_addr.s_addr = INADDR_ANY; //address of localmachine
	//srv.sin_addrs.s_addr = inet_addr("127.0.0.1"); //same as INNADRANNy
	memset(&(srv.sin_zero), 0, 8);


	//setsockopt
	int nOptVal = 1;
	int nOptLen = sizeof(nOptVal);
	nRet = setsockopt(nSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&nOptVal, nOptLen);
	if(!nRet)
	{
		std::cout << "Setosockopt good" << std::endl;
	}
	else 
	{
		std::cout << "Setsockopt failed" << std::endl;
	}
	//blocking and non blocking work without this aswell



	//bind the socket to the local port
	nRet = bind(nSocket, (struct sockaddr*)&srv, sizeof(sockaddr_in));
	if(nRet == -1)
	{
		perror("bind");
		return (1);
	}
	std::cout << "Bind is successfull and it is binding to local port " <<
		"retValue is " << nRet << std::endl;

	//Listen the request from client (quees the requests)
	nRet = listen(nSocket, 5); //how many request can be in que
	if(nRet == -1 )
	{
		perror("lsiten failed");
	}
	std::cout << "Listen is successfull for " << nSocket << " on port " << port << std::endl; 
	//keep waiting for new requests and porceed as per the requeest

	nMaxFd = nSocket;

	struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;



	while (true)
	{

		FD_ZERO(&fr);
		FD_ZERO(&fw);
		FD_ZERO(&fe);

		FD_SET(nSocket, &fr);
		FD_SET(nSocket, &fe);
		
		for(int i = 0; i < 5; i++)
		{
			if(nArrClient[i] != 0)
			{
				FD_SET(nArrClient[i], &fr);
				FD_SET(nArrClient[i], &fe);
			}
		}

		nRet = select(nMaxFd + 1, &fr, &fw, &fe, &tv);
		if(nRet == 0)
		{
			//std::cout << "Nothing on a port " << port << std::endl;
			//timeout expired
			//none socket descriptors are ready
		}
		else if(nRet == -1)
		{
			perror("Select");
		}
		else 
		{
			// std::cout << "Data on port " << port << std::endl;
			// std::cout << "Select return is " << nRet << std::endl;
			//Process the request.

			ProcessTheNewRequest(nSocket, fr);
			// if(FD_ISSET(nSocket, &fe))
			// {
			// 	//excpetion
			// 	std::cout << "There is an exception. Just get away from here " << std::endl;
			// }
			// if(FD_ISSET(nSocket, &fw))
			// {
			// 	std::cout << "Ready to write something" << std::endl;
			// }
			// if(FD_ISSET(nSocket, &fr))
			// {
			// 	std::cout << "Ready to read something. Something new came up at the port" << std::endl;
			// 	//accept new connection
			// }


			//nRet number of fd are ready for read right or exception
			//contains those number on which we have something to do???
			//when someone connect or communicates with messsage over
			//a dedicated connection

		}
		//std::cout << "After the select call" << std::endl;
	}


	close(nSocket);
	return 0;
}
