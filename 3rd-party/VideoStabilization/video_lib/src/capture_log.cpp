#include "stdafx.h"
#include "./../capture_log.h"

Capture_Log* Capture_Log::instance = 0;

LogInterface* Capture_Log::getInstance()
{
	if (instance == 0)
	{
		instance = new Capture_Log();
	}
	return instance;
}

void Capture_Log::log_write(char const* format, ...) const
{
	if (!enabled) return;
	static bool created = false ;
	FILE *f = 0;
	LogInterface* log = Capture_Log::getInstance();
	if (created) 	
	{
		f = fopen(log->get_log_path().c_str(), "a+");
	}
	else
	{
		f = fopen(log->get_log_path().c_str(), "w");
		if (!f)
		{
			return;
		}
		created = true;
		fprintf(f, "Log_start\n");
	}
	if (!f)
	{
		return;
	}
	va_list ap;
	//	fprintf( f, "Error: ");
	va_start( ap, format );
	vfprintf( f, format, ap );
	va_end( ap );
	fclose(f);
}

const std::string& Capture_Log::get_log_path() const
{
	return log_path;
}

void Capture_Log::set_log_path(const std::string& path)
{
	log_path = path;
}