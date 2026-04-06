@echo off
rem Compilation: Zero2 32-bit

rem DEVCLASS = device class (= device name in _device directory)
rem RUNMODE = run mode (module 1=Zero1, 2=Pi2 v1, 3=Zero2 32-bit, 4=Zero2 64-bit)

set DEVCLASS=ZeroPC
set RUNMODE=3

call setup.bat
..\..\..\_c1.bat
