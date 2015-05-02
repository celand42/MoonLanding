@echo off
cls

set DRIVE_LETTER=%1:
set GAME_PATH=%DRIVE_LETTER%\Game_Engine
set PATH=%DRIVE_LETTER%\MinGW\bin;%DRIVE_LETTER%\MinGW\msys\1.0\bin;%GAME_PATH%\Ogre\bin;%GAME_PATH%\boost_1_57_0\stage\lib;%GAME_PATH%\OgreDeps\bin;%GAME_PATH%\bass_2.4;%GAME_PATH%\MyGUI_3.2.2\bin;%GAME_PATH%\lua_5.3.0\bin;c:\Windows;c:\Windows\system32

make GAME_DRIVE_LETTER="%DRIVE_LETTER%"
