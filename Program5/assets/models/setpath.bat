@echo off
cls

set DRIVE_LETTER=%1:
set GAME_PATH=%DRIVE_LETTER%\Game_Engine
set PATH=%GAME_PATH%\MinGW\bin;%GAME_PATH%\MinGW\msys\1.0\bin;%GAME_PATH%\Ogre\bin;c:\Windows;c:\Windows\system32
