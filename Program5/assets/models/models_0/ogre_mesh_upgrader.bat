@echo off
cls

set DRIVE_LETTER=%1:
set MESH=%2
set GAME_PATH=%DRIVE_LETTER%\Game_Engine
set PATH=%DRIVE_LETTER%\MinGW\bin;%DRIVE_LETTER%\MinGW\msys\1.0\bin;%GAME_PATH%\Ogre_1.8.1\bin;c:\Windows;c:\Windows\system32

OgreXMLConverter %MESH%.mesh.xml
OgreMeshUpgrader %MESH%.mesh
