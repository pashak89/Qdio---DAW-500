@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
set LIB=C:\Qt\5.15.2\msvc2019_64\lib;%LIB%
cd /d "C:\Users\AmirPasha\Desktop\Claude Projects\DAW-500\build2019"
qmake "..\daw-3-SampleEditor\SampleEditor.pro" -spec win32-msvc "CONFIG+=debug" "CONFIG+=qml_debug" 2>&1
nmake debug 2>&1
echo BUILD_EXIT=%ERRORLEVEL%
