/*************************************************************************
    > File Name: ServerSocket.h
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 07:10:32 AM PDT
 ************************************************************************/

#ifndef SERVERSOCKET_H__
#define SERVERSOCKET_H__

#include "Socket.h"
#include "ThreadReadWriteLock.h"

#include <list>

class ServerSocket : public Socket
{
public:
	explicit ServerSocket();
	explicit ServerSocket(const int port);
	~ServerSocket();

	void Accept(Socket& socket);
	void Run();

private:
	bool Accept();
	void AddClient(Socket* clientSocket);

	static void* ProcessMessage(void *arg);
	static void DeleteClient(Socket* clientSocket);
	static void SendToAllUsers(const std::string& message);
	
private:
	static std::list<Socket*> m_clientList;
	static ThreadReadWriteLock m_threadRWLock;
};
#endif
