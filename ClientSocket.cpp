/*************************************************************************
    > File Name: ClientSocket.cpp
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 07:25:53 AM PDT
 ************************************************************************/

#include "ClientSocket.h"

ClientSocket::ClientSocket(const std::string& host, const int port)
{
	if(!Socket::Create())
		throw SocketException("Could not create client socket.");
	if(!Socket::Connect(host, port))
		throw SocketException("Could not connect to port.");
    //SetNonBlocking(true);
}

ClientSocket::~ClientSocket()
{

}

int ClientSocket::Send(const std::string& message)
{
//	std::cout << "the message of send is " << message << std::endl;
	return Socket::Send(static_cast<Socket&>(*this), message);
}


int ClientSocket::Receive(std::string& message)
{
	return Socket::Receive(static_cast<Socket&>(*this), message);
}
