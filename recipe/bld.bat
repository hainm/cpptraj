setlocal enableextensions enabledelayedexpansion

@echo off

:: Setting variables in Cygwin style
set LIBRARY_INC_CW=!LIBRARY_INC:\=/!
set LIBRARY_INC_CW=!LIBRARY_INC_CW::=!
set LIBRARY_INC_CW=/%LIBRARY_INC_CW%

set LIBRARY_LIB_CW=!LIBRARY_LIB:\=/!
set LIBRARY_LIB_CW=!LIBRARY_LIB_CW::=!
set LIBRARY_LIB_CW=/%LIBRARY_LIB_CW%

bash configure --with-netcdf=%LIBRARY_INC_CW% ^
               --with-blas=%LIBRARY_INC_CW% ^
               -openblas ^
               --with-bzlib=%LIBRARY_INC_CW% ^
               --with-zlib=%LIBRARY_INC_CW% ^
               -shared ^
               -noarpack ^
               gnu
C:/msys64/usr/bin/make libcpptraj
mkdir -p %PREFIX%/include/cpptraj/
cp lib/libcpptraj* %PREFIX%/lib/
cp src/*.h %PREFIX%/include/cpptraj/
