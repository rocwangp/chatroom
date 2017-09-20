/*************************************************************************
    > File Name: EpollServerSocket.h
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2017年09月09日 星期六 11时34分09秒
 ************************************************************************/

#include "Epoll.h"
#include "Socket.h"

#include <map>

class EpollServerSocket : public Socket
{
	public:
		explicit EpollServerSocket(const int port);
		~EpollServerSocket();

		void Run();

	private:
		bool AddClient(Socket& clientSocket);
		bool DeleteClient(int sockfd);

		void ProcessMessage(Socket& clientSocket);
		
		void ReceiveMessage(Socket& clientSocket, std::string& message);
		void SendMessage(Socket& clientSocket, const std::string& message);

		void SendToAllUsers(const int sockfd, const std::string& message);
        
        static void Handle_Sigint(int);
	private:
		Epoll m_epoll;
		static std::map<int, Socket*>  m_clientMap;
};
