#pragma once

#include <string>
#include <ObjBase.h>

interface LogInterface
{
public:
	virtual const std::string& get_log_path() const = 0;

	virtual void set_log_path(const std::string& path) = 0;

	virtual void log_write(char const* format, ...) const = 0;

};