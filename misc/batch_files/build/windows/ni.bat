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
if "%1"=="d" goto data_flag
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
:data_flag
set TARGET="d"
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

if %TARGET%=="t" goto tgui4
if %TARGET%=="s" goto shim2
if %TARGET%=="w" goto wedge2
if %TARGET%=="g" goto dogo
if %TARGET%=="d" goto data

echo Invalid target: %TARGET%
goto done

:tgui4
cd c:\users\trent\code\tgui4\build
if %CFG%=="release" goto tgui4_release
rem copy relwithdebinfo\tgui4.dll ..\..\d
goto done
:tgui4_release
rem copy release\tgui4.dll ..\..\d
goto done
:shim2
cd c:\users\trent\code\shim2\build
if %CFG%=="release" goto shim2_release
rem copy relwithdebinfo\shim2.dll ..\..\d
goto done
:shim2_release
rem copy release\shim2.dll ..\..\d
goto done
:wedge2
cd c:\users\trent\code\wedge2\build
if %CFG%=="release" goto wedge2_release
rem copy relwithdebinfo\wedge2.dll ..\..\d
goto done
:wedge2_release
rem copy release\wedge2.dll ..\..\d
goto done
:dogo
cd c:\users\trent\code\dog-o\build
if %CFG%=="release" goto dogo_release
copy "relwithdebinfo\dog-o.exe" ..\..\d
goto done
:dogo_release
copy "release\dog-o.exe" ..\..\d
goto done
:data
if %CFG%=="release" goto data_release
cd c:\users\trent\code\d
xcopy /q /e /y ..\dog-o\data data\
copy ..\dog-o\docs\3rd_party.html .
copy ..\dog-o\docs\manual_english.html .
copy ..\dog-o\docs\manual_french.html .
goto done
:data_release
cd c:\users\trent\code\dog-o\data
c:\users\trent\code\compress_dir\compress_dir.exe > nul
move ..\data.cpa c:\users\trent\code\d
copy ..\docs\3rd_party.html c:\users\trent\code\d
copy ..\docs\manual_english.html c:\users\trent\code\d
copy ..\docs\manual_french.html c:\users\trent\code\d
goto done
:done
endlocal
popd
