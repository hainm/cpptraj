bash configure --with-netcdf=%PREFIX% --with-blas=%PREFIX% --with-bzlib=%PREFIX% --with-zlib=%PREFIX% -shared -nomathlib gnu -openblas -noarpack gnu
make libcpptraj -j4
mkdir -p %PREFIX%/include/cpptraj/
cp lib/libcpptraj* %PREFIX%/lib/
cp src/*.h %PREFIX%/include/cpptraj/
