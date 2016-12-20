ls %LIBRARY_PREFIX%
ls %LIBRARY_PREFIX%/include/
bash configure --with-netcdf=%LIBRARY_PREFIX% --with-blas=%LIBRARY_PREFIX% --with-bzlib=%LIBRARY_PREFIX% --with-zlib=%LIBRARY_PREFIX% -shared -nomathlib gnu -openblas -noarpack gnu
C:/msys64/usr/bin/make libcpptraj
mkdir -p %PREFIX%/include/cpptraj/
cp lib/libcpptraj* %PREFIX%/lib/
cp src/*.h %PREFIX%/include/cpptraj/
