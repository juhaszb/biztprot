#ifndef _parse_h_
#define _parse_h_
#include "message.h"
#include "messagetype.h"
#include "connection.h"
#include <filesystem>
#include <vector>
#include <cryptopp/secblock.h>
#include <cryptopp/scrypt.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

class Parser{
	private:
		std::vector<std::string> split(std::string s,std::string delim)
		{
			std::string token;
			size_t pos;
	
			std::vector<std::string> splits; 

			while((pos =  s.find(delim)) != std::string::npos)
			{
				token = s.substr(0,pos);
				splits.push_back(token);
				s.erase(0,pos+delim.length());
			}
			splits.push_back(s);

			return splits;
		}
		

	public:
	/**
	* Parses the message and constructs a reply message 
	*/

	

	Message parse(Message m, std::shared_ptr<Connection> c){
		switch(m.getType())
		{
			case LOGIN:
			{
				std::vector<std::string> spl = split(m.getData(),";");
				if(spl.size() != 2 )
				{
					c->incrementServerTS();
					return Message(std::string{1,ERROR},"Error with data length",c->GetServerTS(),0);
				}
				std::string username = spl[0];
				std::string password = spl[1];

				
				if(!std::filesystem::exists(username))
				{
					c->incrementServerTS();
					return Message(std::string{1,ERROR},"Error with username or password",c->GetServerTS(),0);
				}
				std::ifstream ifstr("users.txt",std::ios::in);
				
				std::string line;

				while(std::getline(ifstr,line))
				{
					std::vector<std::string> sp = split(line,"\t");
					if(username == sp[0])
					{

						std::string hashed = sp[2];
						std::string salt = sp[1];

						
						std::string sal;

						std::cout<<"sp length:" <<sp[1].size() <<std::endl;	
						CryptoPP::StringSource(sp[1],true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(sal)));

						
						
						CryptoPP::SecByteBlock passw(64);
						CryptoPP::SecByteBlock st((const unsigned char*)&sal[0],64);
						
						


						std::string calcsalt;
						CryptoPP::StringSource(st,64,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(calcsalt)));
						std::cout<<"The calculated salt"<<calcsalt<<std::endl;
						std::cout <<"The password we got"<<password<<std::endl;

						CryptoPP::Scrypt scrypt;
						scrypt.DeriveKey(passw,passw.size(),(const unsigned char*)&password[0],password.size(),st,st.size(),1024,8,16);

						
						std::string passwhex;
						
						
						CryptoPP::StringSource(passw,64,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(passwhex)));

						std::cout <<"The salt read in:" <<salt << std::endl;

						if(hashed == passwhex)
						{	
							c->Login();
							ifstr.close();
							c->incrementServerTS();
							return Message(std::string{1,LOGIN},"Login successfull",c->GetServerTS(),0);
						}
						else
						{
							ifstr.close();
							std::cout<<"Hashed value is" <<hashed<<std::endl;
							std::cout<<"Calculated value is"<<passwhex<<std::endl;
							c->incrementServerTS();
							return Message(std::string{1,ERROR},"Error with password",c->GetServerTS(),0);
						}
					}
				}

			return Message(std::string{1,LOGIN},"LOGIN successfull",0,0);
			break;
			}
			case REGISTER:
			{
				std::vector<std::string> spl = split(m.getData(),";");
				if(spl.size() != 2)
				{
					c->incrementServerTS();
					return Message(std::string{1,ERROR},"Error with data length",c->GetServerTS(),0);
				}
				std::string username = spl[0];
				std::string password = spl[1];
	

				if(std::filesystem::exists(username))
				{
					c->incrementServerTS();
					return Message(std::string{1,ERROR},"Username already exists",c->GetServerTS(),0);
				}
				std::filesystem::create_directory(username);

				std::ofstream outf ("users.txt",std::ios::app);
			
			
				CryptoPP::SecByteBlock derived(64);
				CryptoPP::SecByteBlock salt(64);

				CryptoPP::AutoSeededRandomPool prng;
				prng.GenerateBlock(salt, salt.size());

				CryptoPP::Scrypt scrypt;
				scrypt.DeriveKey(derived,derived.size(),(const unsigned char*)&password[0],password.size(),salt,salt.size(),1024,8,16);
			

				std::string passwhash;
				std::string saltstring;
				CryptoPP::StringSource(derived,64,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(passwhash)));

				CryptoPP::StringSource(salt,64,true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(saltstring)));
				

				CryptoPP::SecByteBlock der(64);
				scrypt.DeriveKey(der,der.size(),(const unsigned char*)&password[0],password.size(),salt,salt.size(),1024,8,16);

				std::string phs;
				CryptoPP::StringSource(der,16,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(phs)));

				std::cout<<"The hash: "<< phs<<std::endl;
				std::cout<<"the password we save:" <<password<<std::endl;


				std::cout<<"The salt we save while register: " <<saltstring <<std::endl;
				std::cout<<"The hash we save while reg: " <<passwhash <<std::endl;

				std::string to_write = username+"\t"+saltstring+"\t"+ passwhash+"\n";

				outf.write(to_write.c_str(),strlen(to_write.c_str()));		
				
				c->Login();
				
				outf.close();
				c->incrementServerTS();
				return Message(std::string{1,REGISTER},"REGISTER succeffull",c->GetServerTS(),0);		
				break;
			}
		}
		c->incrementServerTS();
		return Message(std::string{1,ERROR},"Unkown command",c->GetServerTS(),0);

	}




};


#endif
