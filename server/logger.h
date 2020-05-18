#ifndef LOGGER_H_
#define LOGGER_H_

#include <chrono>
#include <memory>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <mutex>


class Logger
{
private:	
	std::ofstream file;
	std::mutex log_mutex;


	Logger()
	{
		file.open("log.txt",std::ios_base::app);
		
		if(file.fail())
			throw std::ios_base::failure(std::strerror(errno));

	}
	Logger(const Logger&) = delete;
	Logger& operator=(const Logger&) = delete;

public:
	static std::shared_ptr<Logger> getInstance()
	{
		static std::shared_ptr<Logger> logger(new Logger);	
		return logger;
	}

	~Logger()
	{
		file.close();
	}

	void Log(const std::string& log) 
	{

		std::lock_guard<std::mutex> guard(log_mutex);
		{
			std::time_t now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			std::string s(30,'\0');
			std::strftime(&s[0],s.size(),"%Y-%m-%d %H:%M:%S", std::localtime(&now));
		file <<s << log <<std::endl;

		}
	}


};


#endif
