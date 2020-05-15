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
#include "crypto.h"

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
							
							ifstr.close();
							c->incrementServerTS();
							std::filesystem::path cpath = std::filesystem::current_path();
							cpath /= username;
							c->setPath(cpath);
							c->Login();
							std::cout<<"Current pwd"<<cpath<<std::endl;
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
		
			case GWD:
			{
				c->incrementServerTS();
				return Message(std::string{1,GWD},c->getPath().string(),c->GetServerTS(),0);
				break;
			}
			case MKD:
			{
				std::filesystem::path currpath = c->getPath();
				currpath /= m.getData();
				if(std::filesystem::exists(currpath))
				{
					c->incrementServerTS();
					return Message(std::string{1,ERROR},"Directory already exists",c->GetServerTS(),0);
				}
				std::filesystem::path newdirectory = c->getPath();
				newdirectory /= m.getData();
				std::filesystem::create_directory(newdirectory);
				c->incrementServerTS();
				return Message(std::string{1,MKD},"Directory successfully created",c->GetServerTS(),0);
			}
			case RMD:
			{
				std::filesystem::path cpath = c->getPath();
				cpath /= m.getData();
				if(std::filesystem::exists(cpath)&& std::filesystem::is_directory(cpath))
				{
					std::filesystem::remove(cpath);
					c->incrementClientTS();
					return Message(std::string{1,RMD},"Folder deleted",c->GetServerTS(),0);
				}
				else
				{
					c->incrementServerTS();
					return Message(std::string{1,ERROR},"Folder doesn't exist or it isnt a folder",c->GetServerTS(),0);
				}
			}
			case CWD:
			{
				if(m.getData() == ".." && c->getOriginalPath().parent_path() == std::filesystem::current_path())
				{
					c->incrementServerTS();
					return Message(std::string{1,ERROR},"Cant go higher than your own root folder",c->GetServerTS(),0);
				}
				else if(m.getData() != "..")
				{
						
					c->incrementClientTS();
					std::filesystem::path newpath = c->getPath();
					newpath /= m.getData();
					if(std::filesystem::exists(newpath) &&std::filesystem::is_directory(newpath))
					{
					c->setPath(newpath);

					return Message(std::string{1,CWD},"New path:"+newpath.string(),c->GetServerTS(),0);
					}
					else
					{
					return Message(std::string{1,ERROR},"Folder doesnt exist",c->GetServerTS(),0);
					}
				}		
				else if(m.getData() == ".." && c->getOriginalPath().parent_path() != std::filesystem::current_path())
				{
					c->incrementClientTS();
					std::filesystem::path newpath = c->getPath();
					c->setPath(newpath.parent_path());
					return Message(std::string{1,CWD},"New path:"+newpath.string(),c->GetServerTS(),0);
				}
					
			}
			case LST:
			{
				std::string files;
				std::filesystem::path cpath = c->getPath();
				for(const auto & entry : std::filesystem::directory_iterator(cpath))
					files = files+entry.path().string()+"\n";
				c->incrementServerTS();
				return Message(std::string{1,LST},"Files:\n"+files,c->GetServerTS());
			}
			case RMF:
			{
				std::filesystem::path cpath = c->getPath();
				cpath /= m.getData();
				if(std::filesystem::exists(cpath) &&std::filesystem::is_regular_file(cpath))
				{
					std::filesystem::remove(cpath);
					c->incrementServerTS();
					return Message(std::string{1,RMF},"File removed",c->GetServerTS());
				}
				else
				{
					c->incrementServerTS();
					return Message(std::string{1,ERROR},"File doesn't exist",c->GetServerTS());
				}
			}
			case UPL:
			{

				std::string filename;
				int linecount;
				std::vector<std::string> param= split(m.getData(),";");
				std::filesystem::path cpath = c->getPath();
				filename = param[0];
				linecount = std::stoi(param[1]);
				cpath /= filename;
				if(std::filesystem::exists(cpath))
				{
					c->incrementServerTS();
					return Message(std::string{1,ERROR},"File already exists",c->GetServerTS());
				}
				else
				{
					std::ofstream ofs(cpath);
					char buffer[2048] = {0};
					while(linecount > 1)
					{
						
						linecount --;
						c->incrementServerTS();
						Message resp(std::string{1,UPL},"Reading linecount",c->GetServerTS());
						MyCrypto mc(c->getKey());

						std::string resps = mc.encrypt(resp.toByteStream());

						std::string respstring;
						CryptoPP::StringSource(resps,true,new CryptoPP::HexEncoder(new CryptoPP::StringSink(respstring)));
					
					
						send(c->getsocket(),respstring.c_str(),strlen(respstring.c_str()),0);

						read(c->getsocket(),buffer,2048);
						
						std::string ciph;
						CryptoPP::StringSource(std::string{buffer},true,new CryptoPP::HexDecoder(new CryptoPP::StringSink(ciph)));

						std::string plain = mc.decrypt(ciph);	

						ofs.write(plain.c_str(),strlen(plain.c_str()));

					}
					ofs.close();
					c->incrementServerTS();
					return Message(std::string{1,UPL},"File written",c->GetServerTS());
				}
			}
		}
		c->incrementServerTS();
		return Message(std::string{1,ERROR},"Unkown command",c->GetServerTS(),0);

	}




};


#endif
