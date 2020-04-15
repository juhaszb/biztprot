#ifndef EXCEPTION_H_
#define EXCEPTION_H_

#include <exception>

#include <netinet/in.h>

class SockFdFailed: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Failed to create the socket file descriptor";
	}
};


class BindError: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Failed to bind socket";
	}
};


class AcceptError: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Failed to accept";
	}
};

class AddressError: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Invalid Address/Wrong address format";
	}
};


class ConnectError: public std::exception
{
	virtual const char* what() const throw()
	{
		return "Connection failed, server offline or full";
	}
};
#endif
