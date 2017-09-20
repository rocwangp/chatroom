/*************************************************************************
    > File Name: EpollServerSocket.cpp
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2017年09月09日 星期六 12时25分56秒
 ************************************************************************/

#include "EpollServerSocket.h"
#include "SocketException.h"
#include "Socket.h"

#include <iostream>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

std::map<int, Socket*> EpollServerSocket::m_clientMap;

EpollServerSocket::EpollServerSocket(const int port)
{
	if(!Socket::Create())
		throw SocketException("Could not create server socket.");
	if(!Socket::Bind(port))
		throw SocketException("Could not bind to port.");
	if(!Socket::Listen())
		throw SocketException("Could not listen to socket.");
	SetNonBlocking(true);
}


EpollServerSocket::~EpollServerSocket()
{
	for(std::map<int, Socket*>::iterator it = m_clientMap.begin();
			it != m_clientMap.end(); ++it)
	{
        close(it->first);
		delete it->second;
	}
	m_clientMap.clear();
}

void EpollServerSocket::Handle_Sigint(int)
{
    for(std::map<int, Socket*>::iterator it = m_clientMap.begin();
        it != m_clientMap.end(); ++it)
    {
        close(it->first);
        delete it->second;
    }
    m_clientMap.clear();
   // close(Socket::GetSockfd());
    std::cout << "server exit...\n";
    exit(1);
}

void EpollServerSocket::Run()
{
    //first Add socket_fd to Epoll_fd and start mnotor socket_fd
    //event type is EPOLLIN represent whether the socket_fd is ready to read or not
    //when there is a client try to connect to the server, the socket_fd is ready
	if(m_epoll.Add(Socket::GetSockfd(), EPOLLIN) == false)
		return;
	
	Socket *clientSocket = NULL;
    
    signal(SIGINT, Handle_Sigint);
	while(true)
	{
        //Blocking to wait untill there is fd ready in Epoll_fd
        //when first loop, there is only one fd in the Epoll_fd , is socket_fd
        //when there is client connect to server already, the Epoll_fd includes both
        //return the number of ready fd and store all ready struct epoll_event in Epoll::m_Events
        //
        //the difference between select and epoll is
        //the select only monotor whether fd is ready and return the number of ready_fd, 
        //but the project didn't know which fd is ready, so need to use FD_ISSET() to judge
        //
        //the epoll not only return the number, but also return all ready epoll_event(include fd) 
        //so project can directly know which fd is ready and only need to loop traversal Epoll::m_Events
		int number = m_epoll.Wait();
		
		for(int i = 0; i < number; ++i)
		{
            //return the monotor_event for fd : such as EPOLLIN, EPOLLOUT, EPOLLET etc
            //these event can use | operator
			int event = m_epoll.GetEvents(i);

            //return the monotor fd
			int fd = m_epoll.GetEventOccurfd(i);
            
            //when event is error or the event is not wo monotor, error
            //delete the fd from Epoll_fd and delete client from client_map
			if((event & EPOLLERR) || (event & EPOLLHUP) || !(event & EPOLLIN))
			{
				perror("epoll error");
				DeleteClient(fd);
			}
            //when the fd is sever_fd, show that there is client try to connect to server,
            //need to accept the client and add client_fd to Epoll_fd used for monoto the client 
            //and add the client to client_map
			else if(fd == Socket::GetSockfd())
			{
				clientSocket = new Socket;
				AddClient(*clientSocket);
			}
            //otherwise, the fd is client, show that the client is sending message,
            //judge the message to choose to send message to all user or to exit this client
			else
			{
				ProcessMessage(*m_clientMap[fd]);
			}
		}
	}
}


bool EpollServerSocket::AddClient(Socket& clientSocket)
{
    //there is client try to connect to server, need to accept 
    //the scoket::accept return a new fd used for client, need to record the fd in clientSocket
    //and then add the clientSocket to client_map with this new fd
	if(Socket::Accept(clientSocket) == false)
		return false;
	int fd = clientSocket.GetSockfd();
    
    //add client_fd to Epoll_fd used for monoto client 
	if(m_epoll.Add(fd, EPOLLIN | EPOLLET) == false)
		return false;

#ifdef DEBUG
	std::cout << "New Client with sockfd : " << fd << std::endl;
#endif
    
    //add to client_map
	m_clientMap[fd] = &clientSocket;
	return true;
}


bool EpollServerSocket::DeleteClient(int sockfd)
{
    //delete fd from Epoll_fd and cancel monotor this fd
	if(m_epoll.Delete(sockfd) == false)
		return false;
    
    //delete from client_map
	m_clientMap.erase(sockfd);

    //because the fd return by accept is apply memory
    //need to close it prevent system_fd full
	close(sockfd);
#ifdef DEBUG
	std::cout << "Delete a client with sockfd : " << sockfd << std::endl;
#endif

	return true;
}


void EpollServerSocket::ProcessMessage(Socket& clientSocket)
{
    //because the client_fd is ready to read, first to read message
	std::string message;
	ReceiveMessage(clientSocket, message);
    
    //exit this client
	if(message == "exit")
	{
        //send this client "user exit" and delete it
		SendMessage(clientSocket, "user_exit");
		DeleteClient(clientSocket.GetSockfd());
	}
	else
	{
        //send message to all users
		SendToAllUsers(clientSocket.GetSockfd(), message);
	}
}


void EpollServerSocket::ReceiveMessage(Socket& clientSocket, std::string& message)
{
    //because set NONBLOCKING, so recv function isn't wait untill there is message to read
    //so need to use loop untill recv successfully
	while(true)
	{
        //NONBLOCKING recv return three conditions
        //first : when errno return -1
        //      but if set blocking and errno is EINTR or EAGAIN or EWOULDBLOCK, no error, need to recv again
        //second : when disconnect from server or client, return 0 !!!!!!!!!!!!!!!!!!!
        //third : when receive message, return the message_len
		int recvReturn = Socket::Receive(clientSocket, message);
		if(recvReturn == -1)
		{
            if(errno == EINTR || errno == EWOULDBLOCK || errno == EAGAIN)
                continue;
            else
            {
               DeleteClient(clientSocket.GetSockfd());
               break; 
            }
        }
        else if(recvReturn == 0)
        {
            DeleteClient(clientSocket.GetSockfd());
            break;
        }
        //project receive MAXRECEIVE message, so when return len less then it
        //show that the receive is over, there is no message need to receive again
        //break
        //otherwise, need to receive again
        //
        //there is problem, when receive_message len more then MAXRECEIVE !!!!!!!!!!!!!!!!!!!
        //the message return will only save last those
		else if(recvReturn < MAXRECEIVE)
			break;
	}
}


void EpollServerSocket::SendMessage(Socket& clientSocket, const std::string& message)
{
	while(true)
	{
        //like ReceiveMessage : Receive, look up
		int ret = Socket::Send(clientSocket, message);
        if(ret < 0)
		{
			if(errno == EAGAIN || errno == EWOULDBLOCK || errno == EINTR)
			{
		 		sleep(1);
				continue;
			}
			else
            {
                DeleteClient(clientSocket.GetSockfd());
                break;
            }
		}
        else if(ret == 0 && message.size() != 0)
        {
            DeleteClient(clientSocket.GetSockfd());
            break;
        }
		else
	    	break;
	}
}

void EpollServerSocket::SendToAllUsers(const int sockfd, const std::string& message)
{
	for(std::map<int, Socket*>::iterator it = m_clientMap.begin();
			it != m_clientMap.end(); ++it)
	{
		if(it->first != sockfd)
			SendMessage(*(it->second), message);
	}
}
