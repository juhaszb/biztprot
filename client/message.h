#ifndef __message_h_
#define __message_h_

#include <string>

class Message
{
	std::string type;
	std::string data;
	unsigned int timestamp;
	unsigned int sessionId;
public:
	Message(std::string&& type,std::string&& data, unsigned int timestamp,unsigned int sessionId): type{type},data{data},timestamp{timestamp},sessionId{sessionId}{}
	std::string toByteStream()  
	{
		return reinterpret_cast<char*>(this);	
	}
};

#endif
