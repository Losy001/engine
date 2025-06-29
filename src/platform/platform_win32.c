#include <platform/platform.h>

static inline void die(int32_t code)
{
	ExitProcess(code);
}

static inline void message_box(const char* title, const char* message, MessageBoxFlags f)
{
	MessageBoxA(nullptr, message, title, f);
}