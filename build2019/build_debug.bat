@echo off
REM Usage:
REM   build_debug.bat            -> incremental build (skips qmake if Makefile.Debug exists)
REM   build_debug.bat qmake      -> force-run qmake first (do this after editing .pro/.pri)
REM   build_debug.bat clean      -> clean then rebuild
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Auxiliary\Build\vcvars64.bat"
set PATH=C:\Qt\5.15.2\msvc2019_64\bin;C:\Qt\Tools\QtCreator\bin\jom;%PATH%
set LIB=C:\Qt\5.15.2\msvc2019_64\lib;%LIB%
cd /d "C:\Users\AmirPasha\Desktop\Claude Projects\DAW-500\build2019"

set RUN_QMAKE=0
if not exist Makefile.Debug set RUN_QMAKE=1
if /I "%~1"=="qmake" set RUN_QMAKE=1
if /I "%~1"=="clean" set RUN_QMAKE=1

if "%RUN_QMAKE%"=="1" (
    echo [build] Running qmake ...
    qmake "..\daw-3-SampleEditor\SampleEditor.pro" -spec win32-msvc "CONFIG+=debug" "CONFIG+=qml_debug" 2>&1
) else (
    echo [build] Skipping qmake. Pass "qmake" arg to force it.
)

if /I "%~1"=="clean" (
    echo [build] jom clean
    jom -j %NUMBER_OF_PROCESSORS% clean 2>&1
)

echo [build] jom -j %NUMBER_OF_PROCESSORS% debug
jom -j %NUMBER_OF_PROCESSORS% debug 2>&1
echo BUILD_EXIT=%ERRORLEVEL%
