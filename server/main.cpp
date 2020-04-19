#include <iostream>
#include "connection.h"
#include <exception>
#include "logger.h"
#include <sys/socket.h>
#include <sys/time.h>
#include <vector>
#include <algorithm>


int main(int argc, char*argv[])
{
	try
	{

		Logger::getInstance()->Log("Ez tehat mukodik");
		Connection master(18000,INADDR_ANY);

		std::vector<std::shared_ptr<Connection>> clients;

		int max_sd;
		fd_set readfs;

	//	std::shared_ptr<Connection> client =master.Accept();
		while(true)
		{	
			FD_ZERO(&readfs);

			FD_SET(master.getsocket(),&readfs);
			max_sd=master.getsocket();

			for(auto &s: clients)
			{
				int sd = s->getsocket();

				if(sd > 0)
					FD_SET(sd, &readfs);

				if(sd > max_sd)
					max_sd = sd;
			}

			int activity = select(max_sd+1,&readfs, NULL,NULL,NULL);

			if((activity <0))
				std::cout<<"Select Error"<<std::endl; // TODO: exception.
			
			if(FD_ISSET(master.getsocket(), &readfs))
			{
				clients.push_back(master.Accept());

				std::cout<<"New connection"<<std::endl;

			}

			for( auto &s : clients)
			{
				int sd = s->getsocket();

				if(FD_ISSET(sd, &readfs))
				{
					char buffer[1025];
					if((read(sd,buffer,1024) == 0))
					{
						auto it = std::find(clients.begin(),clients.end(),s);
						
						clients.erase(it);
					}
					else
					{
						// Itt kell lekezelni az üteneteket... feldolgozni stb...

					}

				}


			}


		}


	}
	catch(std::exception &e)
	{
		std::cout<< "caught exception"<<std::endl;
	}
	return 0;
}
