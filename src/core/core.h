#pragma once

// platform defines

#if defined _WIN32
	#define _CRT_SECURE_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN
#endif

// cross platform headers

#include <stdint.h>
#include <float.h>
#include <math.h>

// types

typedef int16_t i16vec2 __attribute__((ext_vector_type(2)));
typedef uint16_t u16vec2 __attribute__((ext_vector_type(2)));
typedef uint32_t uvec2 __attribute__((ext_vector_type(2)));
typedef int32_t ivec2 __attribute__((ext_vector_type(2)));
typedef float vec2 __attribute__((ext_vector_type(2)));

typedef int32_t ivec3 __attribute__((ext_vector_type(3))); 
typedef float vec3 __attribute__((ext_vector_type(3)));

typedef float vec4 __attribute__((ext_vector_type(4)));
typedef float quat __attribute__((ext_vector_type(4)));
typedef uint8_t u8vec4 __attribute__((ext_vector_type(4)));

typedef float mat4 __attribute__((matrix_type(4, 4)));

// platform headers

#if defined _WIN32
	#include <windows.h>
	#include <windowsx.h>
	#include <hidusage.h>

	#include <gl/GL.h>
#endif

// platform libraries

#if defined _WIN32
	#pragma comment(lib, "vcruntime")
	#pragma comment(lib, "msvcrt")
	#pragma comment(lib, "ucrt")
	
	#pragma comment(lib, "gdi32")
	#pragma comment(lib, "shell32")
	#pragma comment(lib, "kernel32")
	#pragma comment(lib, "user32")
	#pragma comment(lib, "opengl32")
#endif

// entry point

static inline void entry();


#if defined _WIN32
	#if defined DEBUG
		#define ENTRY_POINT void mainCRTStartup()
	#else 
		#define ENTRY_POINT void WinMainCRTStartup()
	#endif
#else
	#define ENTRY_POINT int main()
	#error only windows supported (for now..)
#endif

ENTRY_POINT
{
	entry();
}