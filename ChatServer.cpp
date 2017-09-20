/*************************************************************************
    > File Name: ChatServer.cpp
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 07:19:48 AM PDT
 ************************************************************************/

#include "Socket.h"
#include "EpollServerSocket.h"
#include "SocketException.h"

#include <string>
#include <iostream>
using namespace std;


int main()
{
	cout << "Running server...\n";
	try
	{
		EpollServerSocket server(9090);
		server.Run();
	}
	catch(SocketException &ex)
	{
		cout << "Exception was caughht:" << ex.Description() << "\nExiting.\n";
	}

	return 0;
}
