#pragma once

// platform defines

#ifdef _WIN32
	#define _CRT_SECURE_NO_WARNINGS
	#define WIN32_LEAN_AND_MEAN
#endif

// cross platform headers

#include <stdint.h>

// platform headers

#ifdef _WIN32
	#include <windows.h>
	#include <windowsx.h>

	#include <gl/GL.h>
	#include <gl/GLU.h>
#endif

// platform libraries

#ifdef _WIN32
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

void entry();

#ifdef _WIN32
	void WinMainCRTStartup() 
	{
		entry();
	}
#endif