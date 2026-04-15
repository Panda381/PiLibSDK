@echo off                     
rem This PiLibSDK library supports only CF_A8, CF_B8G8R8 and CF_A8B8G8R8 formats.
..\..\..\..\_tools\guipic\guipic intro.bmp intro.cpp IntroImg CF_B8G8R8
..\..\..\..\_tools\guipic\guipic sprites.tga sprites.cpp SpritesImg CF_A8B8G8R8
