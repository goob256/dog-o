@echo off
pushd .
setlocal
if "%1"=="t" goto tgui4
if "%1"=="s" goto shim2
if "%1"=="w" goto wedge2
if "%1"=="g" goto dogo
:tgui4
call nb.bat t %2
call ni.bat t %2 %3 %4
goto done
:shim2
call nb.bat s %2
call ni.bat s %2 %3 %4
goto done
:wedge2
call nb.bat w %2
call ni.bat w %2 %3 %4
goto done
:dogo
call nb.bat g %2
call ni.bat g %2 %3 %4
goto done
:done
endlocal
popd
