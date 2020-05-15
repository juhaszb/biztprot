#ifndef CONNECTION_H_
#define CONNECTION_H_

#include <sys/types.h>
#include <netinet/in.h>
#include "exception.h"
#include <string.h>
#include <memory>
#include <unistd.h>
#include "logger.h"
#include <cryptopp/aes.h>
#include <filesystem>
#include <iostream>

#ifndef byte
typedef unsigned char byte;
#endif


class Connection
{
	private:
	char buffer[256];
	struct sockaddr_in connection; // port and other network related stuff
	int sockfd; // socket file descriptor
	bool symmetrickey = false;
	byte key[CryptoPP::AES::MAX_KEYLENGTH]; 
	bool loggedIn = false;
	unsigned int ServerTs;
	unsigned int ClientTs;
	std::filesystem::path pwd;
	std::filesystem::path startingpwd;

	void setsocket(int sock)
	{
		sockfd = sock;
	}

	public:

	Connection() = default;


	Connection(unsigned int portnum,in_addr_t address)
	{
		bzero((char*)&connection,sizeof(connection));
		connection.sin_port=htons(portnum);
		connection.sin_addr.s_addr=address;
		sockfd=socket(AF_INET,SOCK_STREAM,0);
		if(sockfd < 0)
			throw SockFdFailed();
		
		if(bind(sockfd,(struct sockaddr*)&connection, sizeof(connection))< 0)
			throw BindError();
		
		Logger::getInstance()->Log("Ready to listen");
		std::cout<<"ready to listen"<<std::endl; 							
		listen(sockfd,5);
	}


	std::shared_ptr<Connection> Accept()
	{
		Logger::getInstance()->Log("Accepting connections");
		std::cout<<"Accepting connections"<<std::endl;
		int addrlen = sizeof(connection);
		int socket = accept(sockfd,(struct sockaddr *)&connection,(socklen_t*)&addrlen);	
		if(socket < 0)
		{
			throw AcceptError();
		}
		std::shared_ptr<Connection> c =std::make_shared<Connection>();
		c->setsocket(socket);
		Logger::getInstance()->Log("Connected");
		std::cout<<"Connected"<<std::endl;
		return c;
	}

	int getsocket()
	{
		return sockfd;
	}


	unsigned int GetServerTS() const
	{
		return ServerTs;
	}
	unsigned int GetClientTS() const
	{
		return ClientTs;
	}
	void incrementServerTS()
	{
		ServerTs++;
	}
	void incrementClientTS()
	{
		ClientTs++;
	}

	void  setServerTs( unsigned int time)
	{
		ServerTs = time;
	}
	void setClientTs(unsigned int time)
	{
		ClientTs = time;
	}

	void Login()
	{
		loggedIn = true;
	}

	bool isloggedin() const 
	{
		return loggedIn;
	}

	bool hasSymmetricKey()
	{
		return symmetrickey;
	}

	void SetKey(std::string k)
	{
		for(int i = 0 ; i < k.length(); i++)
			key[i] = k [i];
		symmetrickey = true;
	}

	void setPath(std::filesystem::path p)
	{
		if(!loggedIn)
			startingpwd = p;
		pwd=p;
	}

	std::filesystem::path getPath(void)
	{
		return pwd;
	}

	std::filesystem::path getOriginalPath(void)
	{
		return startingpwd;
	}

	byte* getKey()
	{
		return key;
	}
	~Connection()
	{
		close(sockfd);
	}
	
};

#endif	
