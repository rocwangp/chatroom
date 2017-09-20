/*************************************************************************
    > File Name: socket.h
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 05:54:38 AM PDT
 ************************************************************************/

#ifndef _SOCKET_H__
#define _SOCKET_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include <cstdio>
#include <cerrno>
#include <cstdlib>
#include <unistd.h>
#include <memory.h>

#include <iostream>
#include <string>

const int MAXCONNECTION = 5;
const int MAXRECEIVE = 500;

class Socket
{
public:
	Socket();
	virtual ~Socket();

	bool Create();
	bool Bind(const int port);
	bool Listen() const;
	bool Accept(Socket& clientSocket) const;

	bool Connect(const std::string& host, const int port);

	static int Send(Socket& socket, const std::string& message);
	static int Receive(Socket& socket, std::string& message);

	void SetNonBlocking(const bool flag);
	bool isValid() const;
	
	int GetSockfd() const {
		return m_sockfd;
	}
private:
	int m_sockfd;
	struct sockaddr_in m_address;
};

#endif
