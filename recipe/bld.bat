REM %PYTHON% -m pip install numpy
REM %PYTHON% %RECIPE_DIR%\update_configure_win.py

REM bash configure2 --with-netcdf=%PREFIX% ^
REM                -nobzlib ^
REM                --with-zlib=%PREFIX% ^
REM                -shared ^
REM                -nomathlib ^
REM                -noreadline ^
REM                -windows ^
REM                gnu

%PYTHON% %RECIPE_DIR%\make_win_config.py

C:\msys64\usr\bin\make libcpptraj
C:\msys64\usr\bin\make

mkdir -p %LIBRARY_INC%\cpptraj\
COPY bin\cpptraj.exe %LIBRARY_BIN%\
COPY lib\libcpptraj.so %LIBRARY_LIB%\
XCOPY src\*.h   %LIBRARY_INC%\cpptraj
