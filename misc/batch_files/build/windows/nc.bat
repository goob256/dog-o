@echo off
pushd .
setlocal

set TARGET="x"
set CFG_FLAGS=
set STEAMWORKS_FLAGS=
set DEMO_FLAGS=

:beginloop
if "%1"=="debug" goto debug
if "%1"=="release" goto endloop rem not supported in this script
if "%1"=="demo" goto demo
if "%1"=="steam" goto steam
if "%1"=="t" goto tgui4_flag
if "%1"=="s" goto shim2_flag
if "%1"=="w" goto wedge2_flag
if "%1"=="g" goto dogo_flag
if "%1"=="d" goto data_flag
goto doneloop
:debug
set CFG_FLAGS="-DDEBUG=on"
goto endloop
:steam
set STEAMWORKS_FLAGS="-DSTEAMWORKS=on"
goto endloop
:demo
set DEMO_FLAGS="-DDEMO=on"
goto endloop
:tgui4_flag
set TARGET="t"
goto endloop
:shim2_flag
set TARGET="s"
goto endloop
:wedge2_flag
set TARGET="w"
goto endloop
:dogo_flag
set TARGET="g"
goto endloop
:data_flag
set TARGET="d"
goto endloop
:endloop
shift
goto beginloop
:doneloop

if %TARGET%=="t" goto tgui4
if %TARGET%=="s" goto shim2
if %TARGET%=="w" goto wedge2
if %TARGET%=="g" goto dogo
if %TARGET%=="d" goto data

echo Invalid target: %TARGET%
goto done

:tgui4
del c:\users\trent\code\d\tgui4.dll
cd c:\users\trent\code\tgui4
rmdir /s /q build
mkdir build
cd build
c:\users\trent\code\dog-o\misc\batch_files\cmake\windows\tgui4.bat %CFG_FLAGS% %STEAMWORKS_FLAGS%
goto done
:shim2
del c:\users\trent\code\d\shim2.dll
cd c:\users\trent\code\shim2
rmdir /s /q build
mkdir build
cd build
c:\users\trent\code\dog-o\misc\batch_files\cmake\windows\shim2.bat %CFG_FLAGS% %STEAMWORKS_FLAGS%
goto done
:wedge2
del c:\users\trent\code\d\wedge2.dll
cd c:\users\trent\code\shim2
cd c:\users\trent\code\wedge2
rmdir /s /q build
mkdir build
cd build
c:\users\trent\code\dog-o\misc\batch_files\cmake\windows\wedge2.bat %CFG_FLAGS% %STEAMWORKS_FLAGS%
goto done
:dogo
del "c:\users\trent\code\d\dog-o.exe"
cd c:\users\trent\code\dog-o
rmdir /s /q build
mkdir build
cd build
c:\users\trent\code\dog-o\misc\batch_files\cmake\windows\dog-o.bat %CFG_FLAGS% %STEAMWORKS_FLAGS% %DEMO_FLAGS%
goto done
:data
rmdir /s /q c:\users\trent\code\d\data
del c:\users\trent\code\d\data.cpa
cd c:\users\trent\code\d
rmdir /s /q data
goto done
:done
endlocal
popd
