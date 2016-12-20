setlocal enableextensions enabledelayedexpansion

@echo off

set PREFIX=!LIBRARY_PREFIX:\=/!
set PREFIX=!PREFIX::=!
set PREFIX=/%PREFIX%

@echo "ls netcdf.h"
ls %PREFIX%\include\netcdf.h

bash configure --with-netcdf=%PREFIX% ^
               --with-blas=%PREFIX% ^
               -openblas ^
               -nobzlib ^
               --with-zlib=%PREFIX% ^
               -shared ^
               -noarpack ^
               -windows ^
               gnu
               ::--with-bzlib=%PREFIX% ^
C:/msys64/usr/bin/make libcpptraj
mkdir -p %PREFIX%/include/cpptraj/
cp lib/libcpptraj* %PREFIX%/lib/
cp src/*.h %PREFIX%/include/cpptraj/
