@echo off
cls

set DRIVE_LETTER=%1:
set GAME_PATH=%DRIVE_LETTER%\Game_Engine
set PATH=%DRIVE_LETTER%\Game_Engine\MinGW\bin;%DRIVE_LETTER%\Game_Engine\MinGW\msys\1.0\bin;%GAME_PATH%\Ogre\bin;%GAME_PATH%\boost_1_57_0\stage\lib;%GAME_PATH%\OgreDeps\bin;c:\Windows;c:\Windows\system32

make GAME_DRIVE_LETTER="%DRIVE_LETTER%"
