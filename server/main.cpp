#include <iostream>
#include "connection.h"
#include <exception>


int main(int argc, char*argv[])
{
	try
	{
		Connection c(18000);
	}
	catch(std::exception &e)
	{
		std::cout<< "caught exception"<<std::endl;
	}
	return 0;
}
