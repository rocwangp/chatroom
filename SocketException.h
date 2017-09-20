/*************************************************************************
    > File Name: socketException.h
    > Author: 
    > Mail:  
    > Created Time: Thu 07 Sep 2017 06:33:25 AM PDT
 ************************************************************************/

#ifndef SOCKETEXCEPTION_H__
#define SOCKETEXCEPTION_H__

#include <string>

class SocketException
{
public:
	SocketException(std::string description) :
		m_description(description)
	{

	}

	~SocketException()
	{

	}

	std::string Description()
	{
		return m_description;
	}

private:
	std::string m_description;
};

#endif
