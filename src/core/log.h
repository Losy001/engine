#pragma once

#include <core/core.h>

#include <platform/platform.h>

typedef enum : uint8_t
{
	LOG_INFO,
	LOG_WARN,
	LOG_FATAL,
} LogLevel;

static inline void trace_log_impl(LogLevel l, const char* file, uint32_t line, const char* fmt, ...)
{
	char out[2048];
	
	switch (l) 
	{
		case LOG_INFO:  strcat(out, "[INFO]"); break;
		case LOG_WARN:  strcat(out, "[WARN]"); break;
		case LOG_FATAL: strcat(out, "[FATAL]"); break;
	}
	
	sprintf(out + strlen(out), "[%s:%d]: ", file, line);

	va_list args;
	va_start(args, fmt);
	
	vsprintf(out + strlen(out), fmt, args);
	
	va_end(args);

	switch (l) 
	{
		case LOG_INFO: 
		case LOG_WARN: 
		{ 
			write_console(out); 
		} break;
		
		case LOG_FATAL:
		{
			strcat(out, "\nThe process is terminating."),
			message_box("FATAL", out, MESSAGE_BOX_ICON_ERROR);
		} break;
	}
}

#define LOG_INFO(...) trace_log_impl(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__) 
#define LOG_WARN(...) trace_log_impl(LOG_WARN, __FILE__, __LINE__, __VA_ARGS__) 
#define LOG_FATAL(...) trace_log_impl(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__) 