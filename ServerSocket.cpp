/*************************************************************************
 * 
    > File Name: ServerSocket.cpp
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 07:12:38 AM PDT
 ************************************************************************/

#include "ServerSocket.h"
#include "SocketException.h"

#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <iostream>

std::list<Socket*> ServerSocket::m_clientList;
ThreadReadWriteLock ServerSocket::m_threadRWLock;

ServerSocket::ServerSocket() :
	Socket()
{
	m_threadRWLock.InitRWLock();
	m_clientList.clear();
}

ServerSocket::ServerSocket(const int port) :
	Socket()
{
	m_threadRWLock.InitRWLock();
	m_clientList.clear();

	if(!Socket::Create())
		throw SocketException("Could not create server socket.");
	if(!Socket::Bind(port))
		throw SocketException("Could not bind to port.");
	if(!Socket::Listen())
		throw SocketException("Could not listen to socket.");
//	SetNonBlocking(true);
}

ServerSocket::~ServerSocket()
{
	m_threadRWLock.DestroyRWLock();
}

void ServerSocket::Accept(Socket& socket)
{
	if(!Socket::Accept(socket))
		throw SocketException("Could not accept socket.");
}

bool ServerSocket::Accept()
{
	Socket *clientSocket = new Socket;
	Accept(*clientSocket);
	AddClient(clientSocket);
	
//	std::cout << "has connected one client\n";

	pthread_t tid;
	int ret = pthread_create(&tid, NULL, ProcessMessage, static_cast<void*>(clientSocket));
	if(ret < 0)
		return false;

	ret = pthread_detach(tid);
	if(ret < 0)
	{
		perror("pthread_detach()");
		return false;
	}
	return true;
}

void ServerSocket::Run()
{
	while(true)
	{
		if(m_clientList.size() >= MAXCONNECTION)
			break;
		else
			Accept();
	}
}

void *ServerSocket::ProcessMessage(void *arg)
{
	Socket *clientSocket = static_cast<Socket*>(arg);
	
//	std::cout << "in processMessage()\n";
	Send(*clientSocket, "Welcome!");
	std::string message;
	while(true)
	{
		if(Receive(*clientSocket, message) <= 0)
			continue;
	//	std::cout << "message is " << message << std::endl;
		if(message == "exit")
		{
			Send(*clientSocket, "exit_user");
			DeleteClient(clientSocket);
			break;
		}
		else
		{
			SendToAllUsers(message);
		}
		//sleep(1);
	}
	pthread_exit(NULL);
	return NULL;
}

void ServerSocket::AddClient(Socket* clientSocket)
{
	m_clientList.push_back(clientSocket);
}

void ServerSocket::DeleteClient(Socket* clientSocket)
{
	m_threadRWLock.SetWriteLock();
	for(std::list<Socket*>::iterator it = m_clientList.begin();
			it != m_clientList.end(); ++it)
	{
		if(*it == clientSocket)
		{
			m_clientList.erase(it);
			delete clientSocket;
			break;
		}
	}
	m_threadRWLock.CloseWriteLock();
}


void ServerSocket::SendToAllUsers(const std::string& message)
{
//	std::cout << "in SendToAllUsers\n";
	m_threadRWLock.SetReadLock();
	std::string new_message;
//	std::cout << "get read lock, start to sent to all Users" << std::endl;
	for(std::list<Socket*>::iterator it = m_clientList.begin();
			it != m_clientList.end(); ++it)
	{
		std::cout << (*it)->getSockfd() << std::endl;
		Send(**it, message);
	}
	
	m_threadRWLock.CloseReadLock();
}
