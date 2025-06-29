@echo off
setlocal EnableDelayedExpansion

set FLAGS=-std=gnu2y ^
-Iinclude ^
-Isrc ^
-nostdlib ^
-fuse-ld=lld ^
-fenable-matrix ^
-Wno-undefined-internal ^
-Xlinker -subsystem:windows

clang src/main.c -o bin/trucksim.exe %FLAGS%

if %errorlevel% neq 0 exit /b %errorlevel%

cd bin
call trucksim.exe
cd ..