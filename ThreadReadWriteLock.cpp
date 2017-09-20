/*************************************************************************
    > File Name: ThreadReadWriteLock.cpp
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 09:31:29 AM PDT
 ************************************************************************/

#include "ThreadReadWriteLock.h"
#include <iostream>

ThreadReadWriteLock::ThreadReadWriteLock()
{
}

ThreadReadWriteLock::~ThreadReadWriteLock()
{
}

void ThreadReadWriteLock::InitRWLock()
{
	pthread_mutex_init(&m_readLock, NULL);
	pthread_mutex_init(&m_writeLock, NULL);
}

void ThreadReadWriteLock::DestroyRWLock()
{
	pthread_mutex_destroy(&m_readLock);
	pthread_mutex_destroy(&m_writeLock);
}

bool ThreadReadWriteLock::SetWriteLock()
{
	pthread_mutex_lock(&m_writeLock);
	return true;
}

bool ThreadReadWriteLock::CloseWriteLock()
{
	pthread_mutex_unlock(&m_writeLock);
	return true;
}

bool ThreadReadWriteLock::SetReadLock()
{
//	std::cout << "try to get read lock\n";
	pthread_mutex_lock(&m_readLock);
//	std::cout << "get read lock successfully\n";
	return true;
}


bool ThreadReadWriteLock::CloseReadLock()
{
	pthread_mutex_unlock(&m_readLock);
	return true;
}
