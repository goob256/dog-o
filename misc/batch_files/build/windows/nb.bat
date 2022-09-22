@echo off
pushd .
setlocal

set CFG="relwithdebinfo"
set TARGET="x"

:beginloop
if "%1"=="debug" goto debug
if "%1"=="release" goto release
if "%1"=="demo" goto endloop rem not supported in this script
if "%1"=="steam" goto endloop rem not supported in this script
if "%1"=="t" goto tgui4_flag
if "%1"=="s" goto shim2_flag
if "%1"=="w" goto wedge2_flag
if "%1"=="g" goto dogo_flag
if "%1"=="d" goto endloop rem not supported by this script
goto doneloop
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
:release
set CFG="release"
goto endloop
:debug
set CFG="relwithdebinfo"
goto endloop
:endloop
shift
goto beginloop
:doneloop

if %TARGET%=="t" goto tgui4_type
if %TARGET%=="s" goto shim2_type
if %TARGET%=="w" goto wedge2_type
if %TARGET%=="g" goto dogo_type

:tgui4_type
if %CFG%=="release" goto tgui4_release
goto tgui4

:shim2_type
if %CFG%=="release" goto shim2_release
goto shim2

:wedge2_type
if %CFG%=="release" goto wedge2_release
goto wedge2

:dogo_type
if %CFG%=="release" goto dogo_release
goto dogo

echo Invalid target: %TARGET%
goto done

:tgui4
if "%2"=="r" goto tgui4_release
cd c:\users\trent\code\tgui4\build
msbuild /p:configuration=relwithdebinfo tgui4.sln
goto done
:tgui4_release
cd c:\users\trent\code\tgui4\build
msbuild /p:configuration=release tgui4.sln
goto done
:shim2
if "%2"=="r" goto shim2_release
cd c:\users\trent\code\shim2\build
msbuild /p:configuration=relwithdebinfo shim2.sln
goto done
:shim2_release
cd c:\users\trent\code\shim2\build
msbuild /p:configuration=release shim2.sln
goto done
:wedge2
if "%2"=="r" goto wedge2_release
cd c:\users\trent\code\wedge2\build
msbuild /p:configuration=relwithdebinfo wedge2.sln
goto done
:wedge2_release
cd c:\users\trent\code\wedge2\build
msbuild /p:configuration=release wedge2.sln
goto done
:dogo
if "%2"=="r" goto dogo_release
cd c:\users\trent\code\dog-o\build
msbuild /p:configuration=relwithdebinfo "dog-o.sln"
goto done
:dogo_release
cd c:\users\trent\code\dog-o\build
msbuild /p:configuration=release "dog-o.sln"
goto done
:done
endlocal
popd
