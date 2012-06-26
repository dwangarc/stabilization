#pragma once

#include <stdio.h>
#include "LogInterface.h"

class Capture_Log : public LogInterface
{
	static Capture_Log* instance;
	std::string log_path;
	bool enabled;
protected:
	Capture_Log()
	{
		log_path = "Capture.out";
		enabled = true;
	}
public:
	static LogInterface* getInstance();

	const std::string& get_log_path() const;

	void set_log_path(const std::string& path);

	void log_write(char const* format, ...) const;

	void enable()
	{
		enabled = true;
	}
	void disable()
	{
		enabled = false;
	}

};
