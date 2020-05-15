#ifndef __message_h_
#define __message_h_

#include <string>
#include <iostream>
#include <vector>
class Message
{
	std::string type;
	std::string data;
	 int timestamp;
	 int sessionId;
public:
	Message(std::string type,std::string data,  int timestamp=0, int sessionId=0): type{type},data{data},timestamp{timestamp},sessionId{sessionId}{}
	std::string toByteStream()  
	{
		return type+"|"+data+"|"+std::to_string(timestamp)+"|"+std::to_string(sessionId);
	}

	void setTimestamp(unsigned int ts)
	{
		timestamp = ts;
	}

	void setSessionId(unsigned int sid)
	{
		sessionId = sid;
	}

	static Message fromString(std::string msg,std::string delimiter="|")
	{
	  size_t  pos = 0;
	  std::string token;
	  std::vector<std::string> tokens;
	  while( (pos=msg.find(delimiter)) != std::string::npos)
	  {
		token = msg.substr(0,pos);
		//std::cout<< token <<std::endl;
		tokens.push_back(token);
		msg.erase(0,pos+delimiter.length());
	
	  }
	  //std::cout<<msg<<std::endl;
	  tokens.push_back(msg);
	  Message m{tokens[0],tokens[1],std::stoi(tokens[2]),std::stoi(tokens[3])};
	  //Message m{"bla","bla",0,0};
	  return m;
	}

	//Getters and setter

	char getType(){
		return type[0];
	}

	std::string getData(){
		return data;
	}

	int getTimestamp(){
		return timestamp;
	}

	int getSessionId(){
		return sessionId;
	}
};

#endif
