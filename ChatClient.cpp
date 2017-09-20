/*************************************************************************
    > File Name: ChatClient.cpp
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 07:30:32 AM PDT
 ************************************************************************/

#include "ClientSocket.h"
#include "SocketException.h"

#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>
#include <string>
#include <iostream>
using namespace std;

static void *sendData(void *arg)
{
	ClientSocket *clientSocket = static_cast<ClientSocket*>(arg);
	std::string message;

	while(true)
	{
		getline(cin, message);
        int ret = clientSocket->Send(message);
        //return 0 when disconnect
	    if(ret == 0 && message.size() != 0)
        {
             std::cout << "Disconnect from the server\n";
             break;
        }
        //return -1 when error
        else if(ret < 0)
        {
            break;
        }
        if(message == "exit")
            break;
    }
	pthread_exit(NULL);
}

int main()
{
	cout << "Running client...\n";

	try
	{
		ClientSocket clientSocket("127.0.0.1", 9090);
	//	cout << "create a client and connect to server\n";
		pthread_t tid;
		pthread_create(&tid, NULL, sendData, &clientSocket);
		pthread_detach(tid);
		string message;

        int ret ;
		while(true)
		{
			if(pthread_kill(tid, 0) == ESRCH)
				break;
 
			if((ret = clientSocket.Receive(message)) > 0)
            {
                std::cout << message << std::endl;
            }
            else if(ret == 0)
            {
                std::cout << "Disconnect from server\n";
                break;
            }
            else
                break;
        }
	}
	catch(SocketException& ex)
	{
		cout << "Exception was caugth: " << ex.Description() << endl;
	}
    std::cout << "client exit...\n";
   // exit(0);
	return 0;
}
