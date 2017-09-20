/*************************************************************************
    > File Name: ClientSocket.h
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 07:23:26 AM PDT
 ************************************************************************/

#ifndef CLIENTSOCKET_H__
#define CLIENTSOCKET_H__

#include "Socket.h"
#include "SocketException.h"

#include <string>

class ClientSocket : public Socket
{
public:
	ClientSocket(const std::string& host, const int port);
	~ClientSocket();

	int Send(const std::string& message);
	int Receive(std::string& message);
};
#endif
