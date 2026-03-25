@echo off
rem This PiLibSDK library supports only CF_A8 and CF_A8B8G8R8 formats.
guipic Clouds.bmp Clouds.c CloudsImg CF_A8B8G8R8
guipic flag.bmp flag.c FlagImg CF_A8B8G8R8
guipic mast.tga mast.c MastImg CF_A8B8G8R8
