setlocal
@echo off

SET THIS_SCRIPT_PATH=%~dp0
SET CURR_DIR=%cd%
cd %THIS_SCRIPT_PATH%

set FFSCRIPT_ROOT=%1
IF '%FFSCRIPT_ROOT%'=='' (
	echo "ffscript library path is not specified"
	echo "usage: configure <ffscript root path>"
	goto :EXIT_SCRIPT
)

set FFSCRIPT_ROOT=%FFSCRIPT_ROOT:\=/%

cmake.exe -DFFSCRIPT_ROOT=%FFSCRIPT_ROOT% .

:EXIT_SCRIPT

cd %CURR_DIR%
endlocal