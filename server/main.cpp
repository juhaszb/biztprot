#include <cryptopp/filters.h>
#include <iostream>
#include "connection.h"
#include <exception>
#include "logger.h"
#include <sys/socket.h>
#include <sys/time.h>
#include <vector>
#include <algorithm>
#include "generate.h"
#include <filesystem>
#include "message.h"
#include <cryptopp/aes.h>
#include <cryptopp/rsa.h>
#include <cryptopp/osrng.h>
#include "crypto.h"
#include <cryptopp/hex.h>
#include "parser.h"

#ifndef byte
typedef unsigned char byte;
#endif

int main(int argc, char*argv[])
{
	CryptoPP::AutoSeededRandomPool prng;
	//byte key[CryptoPP::AES::MAX_KEYLENGTH];
	CryptoPP::RSA::PrivateKey pkey;
	CryptoPP::AutoSeededRandomPool rng;

	CryptoPP::InvertibleRSAFunction params;
	params.GenerateRandomWithKeySize(rng,4096);

	LoadPrivateKey("private.key",pkey);

	CryptoPP::RSAES_OAEP_SHA_Decryptor d(pkey);


	if(!std::filesystem::exists("public.key") || !std::filesystem::exists("private.key"))
	{
		generate_rsa_tofile("public.key","private.key");
		Logger::getInstance()->Log("Generating new keys, because one of the keys is missing");
	}
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
					char buffer[2048];
					if((read(sd,buffer,2047) == 0))
					{
						auto it = std::find(clients.begin(),clients.end(),s);
							
						clients.erase(it);
					
						std::cout<<"somebody disconnected"<<std::endl;
					}
					else
					{


						if(!s->hasSymmetricKey())
						{

						//std::cout<<"The message we got was"<<buffer <<std::endl;
						std::cout<<"length is :"<< strlen(buffer) <<std::endl;
						std::cout<<buffer<<std::endl;
						std::string decoded;
						CryptoPP::StringSource s2(std::string{buffer},true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(decoded)));
						std::string recovered;

						CryptoPP::StringSource ss(std::string{decoded},true,
								new CryptoPP::PK_DecryptorFilter(rng,d,new CryptoPP::StringSink(recovered)));

						//std::cout<<recovered<<std::endl;
						std::cout<<recovered.length()<<std::endl;
						s->SetKey(recovered);

						send(sd,"Message got",strlen("Message got"),0);

						memset(buffer,0,2048);

						}
						else
						{
							//CryptoPP::AutoSeededRandomPool temp;
							//temp.GenerateBlock(s->getKey(),CryptoPP::AES::MAX_KEYLENGTH);
							MyCrypto m(s->getKey());
							
							std::cout<<buffer<<std::endl;

							std::string recieved;
							CryptoPP::StringSource dec(std::string{buffer},true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(recieved)));
							std::string ptext = m.decrypt(recieved);
							
							Parser p;
							Message resp = p.parse(Message::fromString(ptext),s);
								


							std::cout<<resp.toByteStream()<<std::endl;

						}
						//Message m = Message::fromString(std::string(buffer));
						// Itt kell lekezelni az üteneteket... feldolgozni stb...

					}

				}


			}


		}


	}
	catch(std::exception &e)
	{
		std::cout<<e.what()<<std::endl;
		std::cout<< "caught exception"<<std::endl;
	}
	return 0;
}
