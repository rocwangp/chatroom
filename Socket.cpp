/*************************************************************************
    > File Name: socket.cpp
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 06:13:38 AM PDT
 ************************************************************************/

#include "Socket.h"

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <memory.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>

#include <iostream>


Socket::Socket() :
	m_sockfd(-1)
{

}

Socket::~Socket()
{
	if(isValid())
		::close(m_sockfd);
}

bool Socket::isValid() const
{
	return m_sockfd != -1;
}

bool Socket::Create()
{
    //create a socket_fd
    //AF_INET(PF_INET) : IPv4 network protocol, Ipv4_address is 32bit , calculate faster then IPv6
    //is convient in local area network commuinate
    //AF_INET6 : IPv6 network protocol, address is 128bit
    //AF_UNIX : Unix local protocol, isn't convient in windows
    //
    //SOCK_STREAM : the data in the form of stream, 
    //if write two times and then read, the data in stream will read out all, and project don't know 
    //the data is written two times
    //SOCK_DGRAM : the data will be group and easy to lost
    //SOCK_RAM : when treat ICMP IP etc will use it
    //
    //0 : choose protoco by system
    //when use SOCK_RAM, the third argument need to choose by project, such as
    //struct protoent *protocol =  getprotobyname("icmp");
    //socket(AF_INET, SOCK_RAM, protocol->p_proto);
	m_sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if(!isValid())
		return false;
	return true;
}

//set port for server_fd, be called by server
bool Socket::Bind(const int port) 
{
    //struct sockaddr_in 
	m_address.sin_family = AF_INET;
    //htons htonl : convert to network byte order
	m_address.sin_port = htons(port);
    //INADDR_ANY : "0.0.0.0", represent  machine_ip that the project run 
	m_address.sin_addr.s_addr = htonl(INADDR_ANY);

    //bind server_fd to sockaddr_in
    int bindReturn = bind(m_sockfd, (struct sockaddr*)&m_address, sizeof(struct sockaddr_in));
	if(bindReturn < 0)
		return false;
	return true;
}

//set monotor number, be called by server
bool Socket::Listen() const
{
	if(!isValid())
		return false;
    //set monotor number, max number of server can accept client
	int listenReturn = listen(m_sockfd, MAXCONNECTION);
	if(listenReturn < 0)
		return false;
	return true;
}

//accept client, be called by server
bool Socket::Accept(Socket& clientSocket) const
{
	if(!isValid())
		return false;

    //when there is no client try to connect, the accept will Blocking
    //when there is client try to connect, the accept will return a new fd used to represent client
    //and then fill in the second argument (struct sockaddr*)
	socklen_t size = sizeof(clientSocket.m_address); //the third argument need to init firstly, but not used!!!!
	clientSocket.m_sockfd = accept(m_sockfd, (struct sockaddr*)&clientSocket.m_address, &size);
	
	return true;
}

//try to connect to server, be called by client
//connect to customized ip(host) and customized port(port)
bool Socket::Connect(const std::string& host, const int port)
{
	if(!isValid())
		return false;
    
    //set sockaddr_in used to connect
	m_address.sin_family = AF_INET;
	m_address.sin_port = htons(port);
    //inet_addr : convert ip_address(string) to ip_address(network byte order)
    //assign to sin_addr.s_addr !!!!!!!!
    //inet_nota : convert ip_address(network byte order) to ip_address(string)
    //assign to sin_addr  !!!!!!!!!1
	m_address.sin_addr.s_addr = inet_addr(host.c_str());

	int connectReturn = connect(m_sockfd, (struct sockaddr*)&m_address, sizeof(struct sockaddr_in));
	if(connectReturn < 0)
		return false;
	return true;
}

int Socket::Send(Socket& socket, const std::string& message) 
{
    //send message to fd
    //when set nonblocking , the send will return immediately
    //MSG_NOSIGNAL : prohibit send error to system, otherwise when network broken, the project will exit
    
    //return value :
    //the same as recv, look Receive
	int result = send(socket.m_sockfd, message.c_str(), message.length(), MSG_NOSIGNAL);
    return result;
 }

int Socket::Receive(Socket& socket, std::string& message) 
{
	char buffer[MAXRECEIVE + 1];
	message.clear();
	memset(buffer, 0, MAXRECEIVE);
    
    //receive message from fd
    //when set nonblocking, the recv will return immediately
    //
    //return value : 
    //0 when disconnect from server or client
    //-1 when error but if set blocking and errno is EINTR or EAGAIN or EWOULDBLOCK, no error and  need to recv again
    //more then 0 when success
    int numberRead = recv(socket.m_sockfd, buffer, MAXRECEIVE, 0);
    if(numberRead < 0)
    {
	    return -1;
	}
    else if(numberRead == 0)
    {
         return 0;
    }
    else
    {
    	message = buffer;
    	return numberRead;
	}
    
}


void Socket::SetNonBlocking(const bool flag)
{
	if(!isValid())
		return;

	int opts = fcntl(m_sockfd, F_GETFL);
	if(opts < 0)
		return;

	if(flag)
		opts |= O_NONBLOCK;
	else
		opts &= (~O_NONBLOCK);
	fcntl(m_sockfd, F_SETFL, opts);
}
