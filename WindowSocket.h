/*************************************************************************
    > File Name: socket.h
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 05:54:38 AM PDT
 ************************************************************************/

#ifndef _SOCKET_H__
#define _SOCKET_H__

#pragma comment(lib,"ws2_32.lib")

#include <string>
#include <Winsock2.h>
#include <windows.h>

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

	static bool Send(Socket& socket, const std::string& message);
	static int Receive(Socket& socket, std::string& message);

	void SetNonBlocking(const bool flag);
	bool isValid() const;
	
	int GetSockfd() const {
		return m_sockfd;
	}
private:
	SOCKET m_sockfd;
	SOCKADDR_IN m_address;
	WORD m_wVersionRequested;
	WSADATA m_wsaData;
};

#endif
