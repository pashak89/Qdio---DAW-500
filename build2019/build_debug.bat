@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
set PATH=C:\Qt\5.15.2\msvc2019_64\bin;%PATH%
cd /d "C:\Users\AmirPasha\Desktop\Claude Projects\DAW-500\build2019"
nmake debug 2>&1
echo BUILD_EXIT=%ERRORLEVEL%
