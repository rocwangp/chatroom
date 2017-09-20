/*************************************************************************
    > File Name: ChatClient.cpp
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 07:30:32 AM PDT
 ************************************************************************/

#include "WindowClientSocket.h"
#include "SocketException.h"

#include <windows.h>
#include <errno.h>
#include <string>
#include <iostream>
using namespace std;

DWORD WINAPI sendData(LPVOID arg)
{
	ClientSocket *clientSocket = static_cast<ClientSocket*>(arg);
	std::string message;
	while(true)
	{
		getline(cin, message);
		clientSocket->Send(message);
		if(message == "exit")
			break;
	}
	ExitThread(0);
}

int main()
{
	cout << "Running client...\n";

	try
	{
		ClientSocket clientSocket("192.168.1.176", 8080);
	//	cout << "create a client and connect to server\n";
		HANDLE handle = CreateThread(NULL, 0, sendData, &clientSocket, 0, NULL);
		string message;
		while(true)
		{
			if(clientSocket.Receive(message) > 0)
				std::cout << message << std::endl;
		}
		
	//	clientSocket.Send(message);
	}
	catch(SocketException& ex)
	{
		cout << "Exception was caugth: " << ex.Description() << endl;
	}

	return 0;
}
