/*************************************************************************
    > File Name: Epoll.h
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2017年09月09日 星期六 11时06分21秒
 ************************************************************************/

#include <sys/epoll.h>
#include <sys/resource.h>

#include "Socket.h"

static const int MAXEPOLLSIZE = MAXCONNECTION + 5;

class Epoll
{
	public:
		Epoll();
		~Epoll();

		bool Add(int sockfd, int eventsOption);
		bool Delete(int sockfd);

		int Wait();
		int GetEventOccurfd(int eventIndex) const;
		int GetEvents(int eventIndex) const;
		
	private:
		int m_epollfd;
		int m_fdNumber;
		struct epoll_event m_event;
		struct epoll_event m_events[MAXEPOLLSIZE];
		struct rlimit m_rt;
};
