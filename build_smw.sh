#!/bin/bash
#
echo "Checking For SDK..."
source "../../setenv.sh"
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchain/steamlink-toolchain.cmake -DUSE_SDL2_LIBS=1 ..
make smw
echo "Build Complete!"
