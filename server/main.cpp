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
#include <ctime>

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

	
	if(!std::filesystem::exists("public.key") || !std::filesystem::exists("private.key"))
	{
		generate_rsa_tofile("public.key","private.key");
		Logger::getInstance()->Log("Generating new keys, because one of the keys is missing");
	}

	LoadPrivateKey("private.key",pkey);

	CryptoPP::RSAES_OAEP_SHA_Decryptor d(pkey);



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
				std::shared_ptr<Connection> newconn = master.Accept();
				
				std::time_t now = std::time(nullptr);
				std::asctime(std::localtime(&now));	
				newconn->setServerTs(now);	
				clients.push_back(newconn);

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
						
						std::string rsadecr;
						CryptoPP::StringSource(decoded,true,new CryptoPP::PK_DecryptorFilter(rng,d,new CryptoPP::StringSink(rsadecr)));

						//std::cout<<decoded <<std::endl;

						std::string keyinhex = Message::fromString(rsadecr).getData();
						std::string key;
						CryptoPP::StringSource(keyinhex,true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(key)));

						//std::string recovered;

						//CryptoPP::StringSource ss(std::string{key},true,
				//				new CryptoPP::PK_DecryptorFilter(rng,d,new CryptoPP::StringSink(recovered)));

						//std::cout<<recovered<<std::endl;
						std::cout<<rsadecr.length()<<std::endl;
						s->SetKey(key);
						s->setClientTs(Message::fromString(rsadecr).getTimestamp());
						
						

						send(sd,"Message got",strlen("Message got"),0);

						memset(buffer,0,2048);

						}
						else
						{
							
							//CryptoPP::AutoSeededRandomPool temp;
							//temp.GenerateBlock(s->getKey(),CryptoPP::AES::MAX_KEYLENGTH);
							MyCrypto m(s->getKey());
							//std::cout <<"Login part"<<std::endl;	
							//std::cout<<buffer<<std::endl;

							std::string recieved;
							CryptoPP::StringSource dec(std::string{buffer},true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(recieved)));
							std::string ptext = m.decrypt(recieved);
							
							if((Message::fromString(ptext).getType() == REGISTER || Message::fromString(ptext).getType() == LOGIN )	&& s->isloggedin())
								continue;

							Parser p;
							Message resp = p.parse(Message::fromString(ptext),s);
						
							std::string respenc = m.encrypt(resp.toByteStream());

							std::string hexenc;
							CryptoPP::StringSource(respenc,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(hexenc)));
							
							send(sd,hexenc.c_str(),strlen(hexenc.c_str()),0);
							memset(buffer,0,2048);

							
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
