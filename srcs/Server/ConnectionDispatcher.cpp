#include "ConnectionDispatcher.hpp"
#include "Socket.hpp"
#include "SocketManager.hpp"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <sys/select.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <csignal>
#include <algorithm>
#include <fcntl.h>
//#include "../Parsing/ParsingUtils.hpp"

volatile sig_atomic_t flag = 0 ;

void handle_sigint(int sig)
{
	flag = 1;
	(void) sig;
}

ConnectionDispatcher::ConnectionDispatcher(SocketManager& sockets, ServersInfo& serversInfo)
:_sockets(sockets), _serversInfo(serversInfo)
{
	_selectTimeout.tv_sec = 0;
	_selectTimeout.tv_usec = 0;
}
ConnectionDispatcher::ConnectionDispatcher(ConnectionDispatcher& source)
:_sockets(source._sockets), _serversInfo(source._serversInfo), _selectTimeout(source._selectTimeout),
_readSetMaster(source._readSetMaster), _writeSetMaster(source._writeSetMaster), _errorSetMaster(source._errorSetMaster)
{

}

ConnectionDispatcher& ConnectionDispatcher::operator=(ConnectionDispatcher& source)
{
	_sockets = source._sockets;
	_serversInfo = source._serversInfo;
	_selectTimeout = source._selectTimeout;
	return(*this);
}

ConnectionDispatcher::~ConnectionDispatcher()
{

}

std::vector<Socket> ConnectionDispatcher::_getAllReadyToReadSockets()
{
	std::vector<Socket> toReturn;
	std::vector<int> listenFD = _sockets.getAllListenFd();
	for(size_t i = 0 ; i < listenFD.size(); i++)
	{
		if(FD_ISSET(listenFD[i], &_readSetTemp))
		{
			Socket &oneSocket = _sockets.getSocketByFd(listenFD[i]);
			toReturn.push_back(oneSocket);
		}
	}
	return toReturn;
	
}

std::vector<int> ConnectionDispatcher::_getReadyToReadCommunicationFds()
{
	std::vector<int> toReturn;
	for(size_t i = 0 ; i < _communicationFds.size(); i++)
	{
		//std::cout << "I am here with fd "  <<_communicationFds[i] << std::endl;
		if(FD_ISSET(_communicationFds[i], &_readSetTemp))
		{
			//std::cout << "Putting " << _communicationFds[i] << "to read set" << std::endl;
			toReturn.push_back(_communicationFds[i]);
		}
	}
	return toReturn;
}



/**
 * @brief FD_SET all filedescriptor of socket for select later
 * 
 */
void ConnectionDispatcher::_setAllServerListenSocketsForRead(void)
{
	std::vector<int> listenFd = _sockets.getAllListenFd();
	for(size_t i = 0; i < listenFd.size(); i++)
	{
		FD_SET(listenFd[i], &_readSetMaster);
	}
}

void ConnectionDispatcher::_handleAllReadySockets(std::vector<Socket>& readySockets)
{
	if(readySockets.empty() == true)
	{
		std::cerr<< "No ReadySockets to handle" << std::endl;
		return;
	}
	for(size_t i =0; i < readySockets.size(); i++)
	{
		Socket& ready = readySockets[i];
		std::cout << "Ready socket is " << std::endl;
		std::cout << ready << std::endl;
		int communicationSocket = ready.getCommunicationSocket();
		FD_SET(communicationSocket, &_readSetMaster);
		std::cout << "put fd: " << communicationSocket << " in set for reading" << std::endl;
		_communicationFds.push_back(communicationSocket);

		// char buffer[1024] = {0};
		// int valread = read( communicationSocket , buffer, 1024);
		// (void) valread;
		// std::cout << buffer << std::endl;
		// const char *http_response = 
		// "HTTP/1.1 200 OK\r\n"
		// "Content-Type: text/html\r\n"
		// "Content-Length: 46\r\n"
		// "Connection: close\r\n"
		// "\r\n"
		// "<html><body><h1>Hello, World!</h1></body></html>";
		// write(communicationSocket , http_response , strlen(http_response));
		//close(communicationSocket);
	}
}


std::string ConnectionDispatcher::_readClientFd(int communicationFd)
{
	//fcntl(communicationFd, F_SETFL, O_NONBLOCK);
	if(FD_ISSET(communicationFd, &_readSetTemp))
	{
		std::cout << "Reay to read" << std::endl;
	}
	std::string fullRequest;
	const int BUFFER_SIZE = 4096;
	char buffer[BUFFER_SIZE];
	while(true)
	{
		if(fullRequest.find("\r\n\r\n") != std::string::npos)
		{
			std::cout << "Data is full" << std::endl;
			break;
		}
		memset(buffer, 0, sizeof(buffer));
		int retVal = recv(communicationFd, buffer, BUFFER_SIZE, MSG_DONTWAIT);
		std::cout << "Ret val is " << retVal << std::endl;
		if(retVal > 0)
		{
			
			fullRequest.append(buffer, retVal);
			//retval is how many bytes are read
		}
		else if(retVal == -1)
		{
			//TODO server error of reading client request remove cerr
			close(communicationFd);
			std::vector<int>::iterator it = std::find(_communicationFds.begin(),
					_communicationFds.end(), communicationFd);
			_communicationFds.erase(it);
			std::cerr<<"Read failed while trying to read client req" << std::endl;
			perror("read");
			break;
		}
		else if(retVal == 0 )
		{
			std::cout << "End of file " << std::endl;
			break;
		}
	}
	FD_CLR(communicationFd, &_readSetMaster);
	std::cout << "fullRequest is [" << fullRequest << "]" << std::endl;
	return fullRequest;
}

void ConnectionDispatcher::_handleAllReadyToReadCommunicationFds
(std::vector<int>& readReadyClientFds)
{
	if(readReadyClientFds.empty() == true)
	{
		std::cout << "0 Ready clients to read" << std::endl;
		return;
	}
	for(size_t i = 0; i < readReadyClientFds.size(); i++)
	{
		//procitaj fd i spremi ga u request
		//ocitaj host i nadi odgovarajuci server 
		//pospremi Serve i commmunication socket u klasu il negdje
		//makni taj communication iz readReady i metni ga u write ready


		int communicationSocket = readReadyClientFds[i];
		/*
			TRY TO FIND communication Socket inside ClientHeader Unreadvector
			if it doesnt exist 
				//creatin new object put it in UnreadVector 
			if it exist 
				readOnce
					if read once == DONE 
						remove commSocket from read readyFD and removed it FROM UNreadVector
					if read once == error
						close connection?? removeid from client fd completely and from UNREAD vector
					if read once == continue
						//should read again //nothihg special?? 
		*/

		_readClientFd(communicationSocket);
		// char buffer[1024] = {0};
		// int valread = read( communicationSocket , buffer, 1024);
		// if(valread == -1)
		// {
		// 	close(communicationSocket);
		// 	std::vector<int>::iterator it = std::find(_communicationFds.begin(),
		// 	_communicationFds.end(), communicationSocket);
		// 	_communicationFds.erase(it);
		// }
		// std::cout << buffer << std::endl;
		// FD_CLR(communicationSocket, &_readSetMaster);
		std::cout << "FERTIG" << std::endl;
		close(communicationSocket);
		// std::vector<int>::iterator it = std::find(_communicationFds.begin(),
		// _communicationFds.end(), communicationSocket);
		// _communicationFds.erase(it);
	}

}

/*
	findwhichfd is ready
	if one of socket fd is ready
		//socket ready Function
		//accept connection
		//store it somewhere
		//set it to readFDMaseter
*/
void ConnectionDispatcher::_handleReadyFd(void)
{
	

	std::vector<Socket> readySockets = _getAllReadyToReadSockets();
	if(readySockets.size() != 0)
	{
		_handleAllReadySockets(readySockets);
		//handle all Sockets;
	}
	std::vector<int> readyReadClientFd = _getReadyToReadCommunicationFds();
	if(readyReadClientFd.size() != 0)
	{
		_handleAllReadyToReadCommunicationFds(readyReadClientFd);
		//ParsingUtils::printVector(readyReadClientFd);
	}

	//ready.getCommunicationSocket();
	// std::cout << "Ready socket is " << std::endl;
	// std::cout << ready << std::endl;
	// int communicationSocket = ready.getCommunicationSocket();
	// char buffer[1024] = {0};
	// int valread = read( communicationSocket , buffer, 1024);
	// (void) valread;
	// std::cout << buffer << std::endl;
	// const char *http_response = 
	// "HTTP/1.1 200 OK\r\n"
	// "Content-Type: text/html\r\n"
	// "Content-Length: 46\r\n"
	// "Connection: close\r\n"
	// "\r\n"
	// "<html><body><h1>Hello, World!</h1></body></html>";
	// write(communicationSocket , http_response , strlen(http_response));
	// close(communicationSocket);

}

int ConnectionDispatcher::_getMaxFd(void) const 
{
	int maxFD = _sockets.getMaxSocketFd();
	for(size_t i = 0 ; i<_communicationFds.size(); i++)
	{
		if(_communicationFds[i] > maxFD)
			maxFD = _communicationFds[i];
	}
	return maxFD;
}

void ConnectionDispatcher::mainLoop(void)
{
	//only those go in select 
	FD_ZERO(&_errorSetMaster);
	FD_ZERO(&_readSetMaster);
	FD_ZERO(&_writeSetMaster);
	signal(SIGINT, handle_sigint); 

	_setAllServerListenSocketsForRead();
	//FD_SET all socketListen fds to those
	while(true)
	{
		if(flag)
		{
			std::cout << "Ctrl + c pressed" << std::endl;
			break;
		}
		_readSetTemp = _readSetMaster;
		_writeSetTemp = _writeSetMaster;
		_errorSetTemp = _errorSetMaster;

		//find maxFD
		//ADD communication socket here as well
		int selectMaxFD = _getMaxFd();
		int retVal = select(selectMaxFD + 1, &_readSetTemp, &_writeSetTemp, &_errorSetTemp, &_selectTimeout);
		if(retVal == -1)
		{
			std::cout << "Select Failed" << std::endl;
		}
		else if(retVal == 0)
		{
			//std::cout << "Nothing is ready yet" << std::endl;
			//nothing is ready
		}
		else 
		{
			_handleReadyFd();
			// std::vector<int> toREad = _sockets.getAllListenFd();
			// void *buffer[1024];
			// read(toREad[0], buffer, 1024);
			//something is ready
			// read
			// move it from current read to next write
		}
	}
}
