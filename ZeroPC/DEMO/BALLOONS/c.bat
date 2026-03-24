@echo off
rem Compilation: All
call d.bat

call c_1.bat
call d_1.bat

call c_3.bat
call d_3.bat

call c_4.bat
call d_4.bat

if exist %TARGET%.IMG del %TARGET%.IMG
