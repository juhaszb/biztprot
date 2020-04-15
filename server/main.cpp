#include <iostream>
#include "connection.h"
#include <exception>

#include <sys/socket.h>

int main(int argc, char*argv[])
{
	try
	{
		Connection c(18000,INADDR_ANY);
		std::shared_ptr<Connection> client =c.Accept();
	}
	catch(std::exception &e)
	{
		std::cout<< "caught exception"<<std::endl;
	}
	return 0;
}
