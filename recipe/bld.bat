::blas?
%PYTHON% -m pip install numpy
%PYTHON% %RECIPE_DIR%\update_configure_win.py

bash configure2 --with-netcdf=%PREFIX% ^
               -nobzlib ^
               --with-zlib=%PREFIX% ^
               -shared ^
               -nomathlib ^
               -noreadline ^
               -windows ^
               gnu

:: %PYTHON% %RECIPE_DIR%\make_win_config.py
C:\msys64\usr\bin\make libcpptraj
C:\msys64\usr\bin\make

mkdir -p %LIBRARY_INC%\cpptraj\
COPY bin\cpptraj.exe %LIBRARY_BIN%\
COPY lib\libcpptraj.so %LIBRARY_LIB%\
XCOPY src\*.h   %LIBRARY_INC%\cpptraj
