@echo off
REM Set the compiler and output executable
SET COMPILER=g++
SET OUTPUT=flappy_bird.exe

REM Source file
SET SOURCE=main.cpp

REM Compile the program
%COMPILER% %SOURCE% -o %OUTPUT% -mwindows

REM Check if compilation was successful
IF %ERRORLEVEL% EQU 0 (
    echo Compilation successful!
    echo Run the program using: %OUTPUT%
) ELSE (
    echo Compilation failed.
)

pause