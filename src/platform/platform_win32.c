#include <platform/platform.h>

static inline void die(int32_t code)
{
	ExitProcess(code);
}

static inline void message_box(const char* title, const char* message, MessageBoxFlags f)
{
	MessageBoxA(nullptr, message, title, f);
}

static inline void write_console(const char* fmt, ...)
{
	char out[2048];

	va_list args;
	va_start(args, fmt);

	int32_t r = vsprintf(out, fmt, args);
	
	va_end(args);

	WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), out, r, nullptr, nullptr);
}