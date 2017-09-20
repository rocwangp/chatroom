/*************************************************************************
    > File Name: Epoll.cpp
    > Author: ma6174
    > Mail: ma6174@163.com 
    > Created Time: 2017年09月09日 星期六 11时15分40秒
 ************************************************************************/

#include "Epoll.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>

Epoll::Epoll() :
	m_fdNumber(0)
{
    //set resource upper limit 
	m_rt.rlim_max = m_rt.rlim_cur = MAXEPOLLSIZE;
	if(setrlimit(RLIMIT_NOFILE, &m_rt) < 0)
	{
		perror("setrlimit");
		exit(1);
	}

    //create epoll, argument is MAX_NUMBER of monitor_fd, return a fd used to epoll_function
	m_epollfd = epoll_create(MAXEPOLLSIZE);
}

Epoll::~Epoll()
{
    //create a epoll_fd will apply memory so need to close it when project over
	close(m_epollfd);
}

bool Epoll::Add(int sockfd, int eventsOption)
{
    //epoll_ctl : used to operate Add, Modify and Delete a fd need to be monotor to epoll_fd
    //first : epoll fd
    //second : operation EPOLL_CTL_ADD , EPOLL_CTL_DELETE and EPOLL_CTL_MOD
    //third : the fd need to be monotor
    //forth : the events need to monoto for fd, such as 
    //EPOLLIN : can be read
    //EPOLLOUT : can be writen
    //EPOLLEL : edge trigger  ???????
    //
    //struct epoll_event : events is the event need to monotor for fd
    //                   : data.fd is the fd need to monotor
	m_event.events = eventsOption;
	m_event.data.fd = sockfd;
	if(epoll_ctl(m_epollfd, EPOLL_CTL_ADD, sockfd, &m_event) < 0)
	{
		perror("epoll_ctrl : add");
		return false;
	}

    //record the number of fd_monotor, used to epoll_wait()
	++m_fdNumber;
	return true;
}


bool Epoll::Delete(int sockfd)
{
    //delete the fd from epoll_fd, the events(forth argument) is not used;
	if(epoll_ctl(m_epollfd, EPOLL_CTL_DEL, sockfd, &m_event) < 0)
	{
		perror("epoll_ctl : delete");
		return false;
	}
	--m_fdNumber;
	return true;
}

int Epoll::Wait()
{
    
    //the third argument is the number of monotor_fd
    //the last argument is timeout, when -1 is blocking wait untill there are fd ready
    //return all ready event(struct epoll_event) in second argument, which is array of struct epoll_event
	int waitReturn = epoll_wait(m_epollfd, m_events, m_fdNumber, -1);
	if(waitReturn < 0)
	{
		perror("epoll_wait");
		exit(1);
	}
	return waitReturn;
}

int Epoll::GetEventOccurfd(int eventIndex) const
{
	return m_events[eventIndex].data.fd;
}

int Epoll::GetEvents(int eventIndex) const
{
	return m_events[eventIndex].events;
}
