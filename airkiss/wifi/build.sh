#!/bin/bash

#set -e

cd branch_16K
make clean
make
cd ..

rm -rf CMakeCache.txt CMakeFiles/ cmake_install.cmake
cmake ./
make



echo "===> outfile is ready !!!"
