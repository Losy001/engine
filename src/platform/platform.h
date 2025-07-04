#pragma once

#include <core/core.h>

typedef enum : uint8_t 
{
	#if defined _WIN32
		MESSAGE_BOX_ICON_ERROR = MB_ICONERROR,
		MESSAGE_BOX_ICON_INFO = MB_ICONINFORMATION,
		MESSAGE_BOX_ICON_WARNING = MB_ICONWARNING,
	#endif
} MessageBoxFlags;

[[clang::overloadable]]
static inline void die(int32_t code);

[[clang::overloadable]]
static inline void message_box(const char* title, const char* message, MessageBoxFlags f);

[[clang::overloadable]]
static inline void write_console(const char* fmt, ...);

[[clang::overloadable]]
static inline void get_error_msg(int32_t err, char* out, size_t n);

[[clang::overloadable]]
static inline int32_t get_last_error();