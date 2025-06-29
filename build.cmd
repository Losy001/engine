@echo off
setlocal EnableDelayedExpansion

set FLAGS=-std=gnu2y ^
-nostdlib ^
-fuse-ld=lld ^
-Iinclude ^
-Isrc ^
-Xlinker -subsystem:windows

clang src/main.c -o bin/trucksim.exe %FLAGS%

if %errorlevel% neq 0 exit /b %errorlevel%

cd bin
call trucksim.exe
cd ..