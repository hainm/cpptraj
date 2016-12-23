#include <cstdio>
#include "netcdf.h"
void unused() {int ncid; nc_open("foo.nc", 0, &ncid);}
int main() { printf("Testing\n"); printf("%s\n",nc_strerror(0)); return 0; }
