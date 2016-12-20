setlocal enableextensions enabledelayedexpansion

@echo off

set LIBRARY_PREFIX_CW=!LIBRARY_PREFIX:\=/!
set LIBRARY_PREFIX_CW=!LIBRARY_PREFIX_CW::=!
set LIBRARY_PREFIX_CW=/%LIBRARY_PREFIX_CW%

ls %LIBRARY_PREFIX_CW%\include

bash configure --with-netcdf=%LIBRARY_PREFIX_CW% ^
               --with-blas=%LIBRARY_PREFIX_CW% ^
               -openblas ^
               --with-bzlib=%LIBRARY_PREFIX_CW% ^
               --with-zlib=%LIBRARY_PREFIX_CW% ^
               -shared ^
               -noarpack ^
               -windows ^
               gnu
C:/msys64/usr/bin/make libcpptraj
mkdir -p %PREFIX%/include/cpptraj/
cp lib/libcpptraj* %PREFIX%/lib/
cp src/*.h %PREFIX%/include/cpptraj/
