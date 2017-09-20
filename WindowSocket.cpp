/*************************************************************************
    > File Name: socket.cpp
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 06:13:38 AM PDT
 ************************************************************************/

#include "WindowSocket.h"


#include <memory.h>

#include <stdlib.h>
#include <stdio.h>
#include <iostream>


Socket::Socket() :
	m_sockfd(-1)
{
	m_wsaData.wVersion = MAKEWORD(1, 1);
	WSAStartup(m_wVersionRequested, &m_wsaData);
}

Socket::~Socket()
{
	if(isValid())
		closesocket(m_sockfd);
}

bool Socket::isValid() const
{
	return m_sockfd != -1;
}

bool Socket::Create()
{
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);
//	std::cout << "server socket fd is " << m_sockfd << std::endl;
	if(!isValid())
		return false;
	return true;
}

bool Socket::Bind(const int port) 
{

	return true;
}


bool Socket::Listen() const
{

	return true;
}

bool Socket::Accept(Socket& clientSocket) const
{
	return true;
}

bool Socket::Connect(const std::string& host, const int port)
{
	if(!isValid())
		return false;

	m_address.sin_family = AF_INET;
	m_address.sin_port = htons(port);
	m_address.sin_addr.S_un.S_addr = inet_addr(host.c_str());

	int connectReturn = connect(m_sockfd, (struct sockaddr*)&m_address, sizeof(SOCKADDR_IN));
	if(connectReturn < 0)
		return false;
	return true;
}

bool Socket::Send(Socket& socket, const std::string& message) 
{
	int result = send(socket.m_sockfd, message.c_str(), message.length(), 0);
	if(result < 0)
		return false;
	return true;
}

int Socket::Receive(Socket& socket, std::string& message) 
{
	char buffer[MAXRECEIVE + 1];
	message.clear();
	memset(buffer, 0, MAXRECEIVE);

	int numberRead = recv(socket.m_sockfd, buffer, MAXRECEIVE, 0);
	if(numberRead < 0)
	{
		std::cout << "error in Socket::Receive\n";
		return -1;
	}
	else if(numberRead == 0)
		return 0;
	else
	{
//		std::cout << "Receive meesage is " << buffer << std::endl;
		message = buffer;
		return numberRead;
	}
}


void Socket::SetNonBlocking(const bool flag)
{
	if(!isValid())
		return;
	int block = 1;
	if (flag)
		ioctlsocket(m_sockfd, FIONBIO, (unsigned long*)&block);

}
