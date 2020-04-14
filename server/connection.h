#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <sys/types.h>
#include <netinet/in.h>
#include "exception.h"
#include <string.h>


#include <iostream>

class Connection
{
	private:
	char buffer[256];
	struct sockaddr_in connection; // port and other network related stuff
	int sockfd; // socket file descriptor

	public:
	Connection(unsigned int portnum)
	{
		bzero((char*)&connection,sizeof(connection));
		connection.sin_port=htons(portnum);
		connection.sin_addr.s_addr =INADDR_ANY;

		sockfd=socket(AF_INET,SOCK_STREAM,0);
		if(sockfd < 0)
			throw SockFdFailed();
		
		if(bind(sockfd,(struct sockaddr*)&connection, sizeof(connection))< 0)
			throw BindError();
		
		std::cout<<"ready to listen"<<std::endl;

		listen(sockfd,5);
	}
	
};

#endif	
