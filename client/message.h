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
	Message(std::string&& type,std::string&& data, unsigned int timestamp=0,unsigned int sessionId=0): type{type},data{data},timestamp{timestamp},sessionId{sessionId}{}
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

	static void fromString()
	{
	  
	  return ;
	}
};

#endif
