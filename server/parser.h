#ifndef _parse_h_
#define _parse_h_
#include "message.h"
#include "messagetype.h"
#include "connection.h"
#include <filesystem>
#include <vector>

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
			return Message(std::string{1,LOGIN},"LOGIN successfull",0,0);
			break;
			}
			case REGISTER:
			{
			std::vector<std::string> spl = split(m.getData(),";");
			if(spl.size() != 2)
			{
				return Message(std::string{1,ERROR},"Error with data length",0,0);
			}
			std::string username = spl[0];
			std::string password = spl[1];
	

			if(std::filesystem::exists(username))
				return Message(std::string{1,ERROR},"Username already exists",0,0);
			
			std::filesystem::create_directory(username);

			std::ofstream outf ("users.txt",std::ios::app);
				
			std::string to_write = username+password;

			outf.write(to_write.c_str(),strlen(to_write.c_str()));		

			return Message(std::string{1,REGISTER},"REGISTER succeffull",0,0);		
			break;
			}
		}
		return Message(std::string{1,ERROR},"Unkown command",0,0);

	}




};


#endif
