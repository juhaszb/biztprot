#ifndef CONNECTION_H_
#define CONNECTION_H_

#include "exception.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <string>
#include <unistd.h>
#include <iostream>


class Connection
{
private:
	int sock;
	struct sockaddr_in serv_addr;
	char buffer[1024] = {0};
public:
	Connection()
	{
		sock=socket(AF_INET,SOCK_STREAM,0);
		if(sock <0)
			throw SockFdFailed();
		serv_addr.sin_family = AF_INET;
		serv_addr.sin_port=htons(18000);
		
		if(inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr)<=0)
			throw AddressError();	
		
		if(connect(sock,(struct sockaddr*)&serv_addr,sizeof(serv_addr)) <0)
			throw ConnectError();
	
	}
	

	void Send(std::string message)
	{
		//const char * hello = "Hello there";
		send(sock,message.c_str(),strlen(message.c_str()),0);
	}

	std::string Read()
	{
		int n = read(sock,buffer,1024);
		if(n<0)
			std::cout<<"Error reading"<<std::endl;
		return std::string{buffer};
	}

	~Connection()
	{
		
		close(sock);
	}
};



#endif
