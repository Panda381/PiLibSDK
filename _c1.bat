@echo off

rem Compilation... Compile one project

rem ---------------------------------------------------
rem Edit this line to setup path to GCC RISC-V compiler
rem ---------------------------------------------------
rem Set path to GCC ARM compiler in files _boot\c.bat and .\_c1.bat
set GCC_PI_PATH=C:\ARM_GCC32\bin;C:\ARM_GCC64\bin

rem Auto-update path
if "%GCC_PI_PATH_ISSET%"=="YES" goto update_ok
set GCC_PI_PATH_ISSET=YES
set PATH=%GCC_PI_PATH%;%PATH%
:update_ok

%~dp0_tools\make.exe all
rem If you want to see all error messages, compile using this command:
rem %~dp0_tools\make.exe all 2> err.txt
if not errorlevel 1 goto end

rem Compilation error, stop and wait for key press
pause ERROR!

:end
