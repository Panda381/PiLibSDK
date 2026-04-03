@echo off
rem This PiLibSDK library supports only CF_A8, CF_B8G8R8 and CF_A8B8G8R8 formats.
..\..\..\..\_tools\guipic\guipic test1.bmp test1.cpp Test1Img CF_B8G8R8
..\..\..\..\_tools\guipic\guipic test2.tga test2.cpp Test2Img CF_A8B8G8R8
..\..\..\..\_tools\guipic\guipic test3.bmp test3.cpp Test3Img CF_A8
