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

static inline void get_error_msg(int32_t err, char* out, size_t n)
{
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, err, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), out, n, nullptr);
}

static inline int32_t get_last_error()
{
	return GetLastError();
}