@echo off
rem Compilation: All
call d.bat

rem Loop to find all sub-directories
for %%d in (c_*.bat) do call :comp1 %%d
if exist %TARGET%.IMG del %TARGET%.IMG
exit /b

rem Compile one build
:comp1
call %1
call d.bat
