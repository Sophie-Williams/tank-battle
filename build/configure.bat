setlocal
@echo off

SET CURR_DIR=%cd%
SET THIS_SCRIPT_PATH=%~dp0
cd %THIS_SCRIPT_PATH%

del CMakeCache.txt

SET PLATFORM=%1
IF '%PLATFORM%'=='' SET PLATFORM=x64

IF /I '%PLATFORM%'=='x64' (
	cmake -DCMAKE_GENERATOR_PLATFORM=x64 ../src
) ELSE (
	cmake ../src
)
cd %CURR_DIR%

endlocal