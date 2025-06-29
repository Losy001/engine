#pragma once

#include <core/core.h>

typedef enum : uint8_t 
{
	#ifdef _WIN32
		MESSAGE_BOX_ICON_ERROR = MB_ICONERROR,
		MESSAGE_BOX_ICON_INFO = MB_ICONINFORMATION,
		MESSAGE_BOX_ICON_WARNING = MB_ICONWARNING,
	#endif
} MessageBoxFlags;

static inline void die(int32_t code);

static inline void message_box(const char* title, const char* message, MessageBoxFlags f);