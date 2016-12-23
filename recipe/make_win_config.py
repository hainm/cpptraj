import os
import sys
import subprocess

config_template = """
CPPTRAJHOME={cpptraj_home}
CPPTRAJBIN={cpptraj_bin}
CPPTRAJLIB={cpptraj_lib}

INSTALL_TARGETS= install_cpptraj install_ambpdb

CC=gcc
CXX=g++
FC=gfortran
CFLAGS= -O3 -Wall  -DNOARPACK -DNO_READLINE -DBINTRAJ -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -I{include_dir} -Ixdrfile  -fPIC $(DBGFLAGS)
CXXFLAGS= -O3 -Wall  -DNOARPACK -DNO_READLINE -DBINTRAJ -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -I{include_dir}  -Ixdrfile  -fPIC -I{include_dir} $(DBGFLAGS)
FFLAGS= -O3  -DNOARPACK -DNO_MATHLIB -DNO_READLINE -DBINTRAJ -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -I{include_dir} -Ixdrfile -ffree-form -fPIC $(DBGFLAGS)
SHARED_SUFFIX=.so

LIBCPPTRAJ_TARGET=$(CPPTRAJLIB)/libcpptraj$(SHARED_SUFFIX)

NVCC=
NVCCFLAGS= $(DBGFLAGS)
CUDA_TARGET=

READLINE=
READLINE_HOME=readline
READLINE_TARGET=noreadline

XDRFILE=xdrfile/libxdrfile.a
XDRFILE_HOME=xdrfile
XDRFILE_TARGET=xdrfile/libxdrfile.a

FFT_DEPEND=pub_fft.o
FFT_LIB=pub_fft.o

CPPTRAJ_LIB=  -L{lib_dir} -lgfortran -lopenblaspy -w
LDFLAGS=-L{lib_dir} -lnetcdf xdrfile/libxdrfile.a 
SFX=
EXE=
"""

include_dir = os.getenv('LIBRARY_INC', os.path.join(sys.prefix, 'Library', 'include'))
# lib_dir = os.getenv('LIBRARY_LIB')
# yeah, weird, right? netcdf.dll is bin folder
lib_dir = os.getenv('LIBRARY_BIN', os.path.join(sys.prefix, 'Library', 'bin'))
cpptraj_home = os.getcwd()
cpptraj_bin = os.path.join(cpptraj_home, 'bin')
cpptraj_lib = os.path.join(cpptraj_home, 'lib')

try:
    os.mkdir('lib')
    os.mkdir('bin')
except OSError:
    pass

with open('config.h', 'w') as fh:
    fh.write(config_template.format(cpptraj_home=cpptraj_home,
        cpptraj_bin=cpptraj_bin,
        cpptraj_lib=cpptraj_lib,
        include_dir=include_dir,
        lib_dir=lib_dir))

filenames_and_flags = [('testp_netcdf.cpp', '-lnetcdf'),
                       ('testp_zlib.cpp', '-lz'),
                       ('testp_openblas.cpp', '-lopenblaspy'),]
                       # ('testp_bzlib.cpp', '-lbzlib')]

this_path = os.path.dirname(os.path.abspath(__file__))
for fn, link_flag in filenames_and_flags:
    test_file = os.path.join(os.getenv('RECIPE_DIR', this_path), fn)
    print(test_file)
    #if fn == 'testp_openblas.cpp':
    #    lib_dir2 = lib_dir.replace('bin', 'lib')
    #else:
    #    lib_dir2 = lib_dir
    command = 'g++ -I{include_dir} -o testp {link_flag} -L{lib_dir} {test_file}'.format(include_dir=include_dir, lib_dir=lib_dir, test_file=test_file, link_flag=link_flag)
    print('command', command)
    subprocess.call(command, shell=True)
