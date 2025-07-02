@echo off
setlocal EnableDelayedExpansion

set FLAGS=-std=gnu2y ^
-Iinclude ^
-Isrc ^
-nostdlib ^
-flto ^
-fuse-ld=lld ^
-fenable-matrix ^
-Wno-undefined-internal

clang src/main.c -o bin/release.exe %FLAGS% -O3 -Xlinker -subsystem:windows
if %errorlevel% neq 0 exit /b %errorlevel%

clang src/main.c -o bin/debug.exe -DDEBUG %FLAGS% -g3 -Xlinker -subsystem:console
if %errorlevel% neq 0 exit /b %errorlevel%

cd bin
call debug.exe
cd ..