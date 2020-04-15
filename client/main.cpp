#include <exception>
#include <iostream>

#include "connection.h"

int main(int argc, char*argv[])
{
	try{
		Connection c;
	}
	catch(std::exception& e)
	{
		std::cout<<"Exception caught"<<std::endl;
	}
	return 0;
}
