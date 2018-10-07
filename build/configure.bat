setlocal
@echo off

SET CURR_DIR=%cd%
SET THIS_SCRIPT_PATH=%~dp0
cd %THIS_SCRIPT_PATH%

cmake -DCMAKE_GENERATOR_PLATFORM=x64 ../src
cd %CURR_DIR%

endlocal