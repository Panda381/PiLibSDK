@echo off
rem Compilation: Zero1

rem DEVCLASS = device class (= device name in _device directory)
rem RUNMODE = run mode (module 1=Zero1, 2=Pi2 v1, 3=Zero2 32-bit, 4=Zero2 64-bit)

set DEVCLASS=PiBase
set RUNMODE=1

call setup.bat
..\..\..\_c1.bat
