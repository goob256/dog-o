@echo off
pushd .
setlocal
if "%1"=="t" goto tgui4
if "%1"=="s" goto shim2
if "%1"=="w" goto wedge2
if "%1"=="g" goto dogo
:tgui4
cd c:\users\trent\code\tgui4
git pull --rebase
goto done
:shim2
cd c:\users\trent\code\shim2
git pull --rebase
goto done
:wedge2
cd c:\users\trent\code\wedge2
git pull --rebase
goto done
:dogo
cd c:\users\trent\code\dog-o
git pull --rebase
goto done
:done
endlocal
popd
