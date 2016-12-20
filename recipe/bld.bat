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

REM C:\msys64\usr\bin\make libcpptraj
REM C:\msys64\usr\bin\make
REM 
REM mkdir -p %LIBRARY_INC%\cpptraj\
REM COPY bin\cpptraj.exe %LIBRARY_BIN%\
REM COPY lib\libcpptraj.so %LIBRARY_LIB%\
REM XCOPY src\*.h   %LIBRARY_INC%\cpptraj
