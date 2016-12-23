setlocal enableextensions enabledelayedexpansion

@echo off

REM Setting variables in Cygwin style
set PREFIX_CW=!PREFIX:\=/!
set PREFIX_CW=!PREFIX_CW::=!
set PREFIX_CW=/%PREFIX_CW%

%PYTHON% %RECIPE_DIR%/update_configure_win.py
bash configure2 --with-netcdf=%PREFIX%/Library ^
               -nobzlib ^
               --with-zlib=%PREFIX%/Library ^
               -shared ^
               -nomathlib ^
               -noreadline ^
               -windows ^
               gnu

REM %PYTHON% %RECIPE_DIR%\make_win_config.py

C:\msys64\usr\bin\make libcpptraj
REM C:\msys64\usr\bin\make

mkdir -p %LIBRARY_INC%\cpptraj\
COPY bin\cpptraj.exe %LIBRARY_BIN%\
COPY lib\libcpptraj.so %LIBRARY_LIB%\
XCOPY src\*.h   %LIBRARY_INC%\cpptraj
