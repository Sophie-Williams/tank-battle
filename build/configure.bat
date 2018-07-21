setlocal
@echo off

SET CURR_DIR=%cd%
SET THIS_SCRIPT_PATH=%~dp0
cd %THIS_SCRIPT_PATH%

cmake -T v140 -G "Visual Studio 15 2017 Win64" ../src ^
        -DCINDER_INCLUDE_DIR=D:/development/personal/myrepo/externals/cinder/include -DCINDER_LIB_DIR=D:/development/personal/myrepo/externals/cinder/lib/vc14/x64
cd %CURR_DIR%

endlocal