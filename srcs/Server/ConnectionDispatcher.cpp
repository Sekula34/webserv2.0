#include "ConnectionDispatcher.hpp"
#include "Socket.hpp"
#include "SocketManager.hpp"
#include <cstddef>
#include <cstring>
#include <sys/select.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <csignal>

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
		char buffer[1024] = {0};
		int valread = read( communicationSocket , buffer, 1024);
		(void) valread;
		std::cout << buffer << std::endl;
		const char *http_response = 
		"HTTP/1.1 200 OK\r\n"
		"Content-Type: text/html\r\n"
		"Content-Length: 46\r\n"
		"Connection: close\r\n"
		"\r\n"
		"<html><body><h1>Hello, World!</h1></body></html>";
		write(communicationSocket , http_response , strlen(http_response));
		close(communicationSocket);
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
		int selectMaxFD = _sockets.getMaxSocketFd();
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
