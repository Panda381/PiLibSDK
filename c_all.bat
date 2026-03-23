@echo off
rem Compile all projects in all sub-directories

cd PiBase
call c.bat
cd ..

cd ZeroPC
call c.bat
cd ..
