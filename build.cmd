@echo off
setlocal EnableDelayedExpansion

set FLAGS=-std=gnu2y ^
-Iinclude ^
-Isrc ^
-nostdlib ^
-fuse-ld=lld ^
-fenable-matrix ^
-Wno-undefined-internal

if [%1] == [gl] (
	pengol 1.5 src\render\render_gl1.c
	move pl_gl.h src\utils\
)

if [%1] == [game] (
	clang src\main.c -o bin\release.exe %FLAGS% -O3 -Xlinker -subsystem:windows
	if %errorlevel% neq 0 exit /b %errorlevel%
	
	clang src\main.c -o bin\debug.exe -DDEBUG %FLAGS% -g -Xlinker -subsystem:console
	if %errorlevel% neq 0 exit /b %errorlevel%

	cd bin
	call debug.exe
	cd ..
)