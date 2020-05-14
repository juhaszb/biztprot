#ifndef _parse_h_
#define _parse_h_
#include "message.h"
#include "messagetype.h"
#include "connection.h"

class Parser{
	public:
	/**
	* Parses the message and constructs a reply message 
	*/
	Message parse(Message m, std::shared_ptr<Connection> c){
		
	}
};


#endif
