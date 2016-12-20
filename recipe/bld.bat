setlocal enableextensions enabledelayedexpansion

@echo off

set PREFIX=!LIBRARY_PREFIX:\=/!
set PREFIX=!PREFIX::=!
set PREFIX=/%PREFIX%

@echo "ls netcdf.h"
ls %PREFIX%\include\netcdf.h

set CROSS_COMPILE="yes"
bash configure --with-netcdf=%PREFIX% ^
               --with-blas=%PREFIX% ^
               -openblas ^
               -nobzlib ^
               --with-zlib=%PREFIX% ^
               -shared ^
               -noarpack ^
               -windows ^
               gnu
               ::--with-bzlib=%PREFIX%

C:\msys64\usr\bin\make libcpptraj
mkdir -p %LIBRARY_INC%\cpptraj\
COPY bin\cpptraj.exe %LIBRARY_BIN%\
COPY lib\libcpptraj.so %LIBRARY_LIB%\
XCOPY src\*.h   %LIBRARY_INC%\cpptraj
