/*************************************************************************
    > File Name: ThreadWriteReadLock.h
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 09:27:27 AM PDT
 ************************************************************************/

#ifndef THREADREADWRITELOCK_H__
#define THREADREADWRITELOCK_H__

#include <pthread.h>

class ThreadReadWriteLock
{
public:
	ThreadReadWriteLock();
	~ThreadReadWriteLock();

	void InitRWLock();
	void DestroyRWLock();

	bool SetWriteLock();
	bool CloseWriteLock();

	bool SetReadLock();
	bool CloseReadLock();

private:
	pthread_mutex_t m_readLock;
	pthread_mutex_t m_writeLock;
};
#endif
