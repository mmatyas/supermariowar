#!/bin/bash
#
echo "Checking For SDK..."
source "../../setenv.sh"
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchain/steamlink-toolchain.cmake -DUSE_SDL2_LIBS=1 ..
make all

# Prepare package directory
rm "./output/smw-steamlink/smw"
rm "./output/smw-steamlink/smw-server"
cp "./Binaries/Release/smw" "../output/smw-steamlink/smw"
cp "./Binaries/Release/smw" "../output/smw-steamlink/smw"
cp "./Binaries/Release/smw-server" "../output/smw"
cp "./Binaries/Release/smw-server" "../output/smw-server"

cd ..
echo "Packaging it for Steam-Link...."
export DESTDIR="${PWD}/output/smw-steamlink"
cd output

# Build TAR.GZ
name=$(basename ${DESTDIR})
tar zcvf $name.tgz $name || exit 3
echo "Build Complete! Check in /output/ directory."

echo "Build Complete!"
