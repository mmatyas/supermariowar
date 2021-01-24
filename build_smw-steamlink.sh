#!/bin/bash
#
echo "Checking For SDK..."
source "../../setenv.sh"
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchain/steamlink-toolchain.cmake -DUSE_SDL2_LIBS=1 ..
make smw

# Prepare package directory
rm "./output/smw-steamlink/smw"

cp "./Binaries/Release/smw" "../output/smw-steamlink/smw"
cp "./Binaries/Release/smw" "../output/smw"

cd ..
rm -r build
echo "Packaging it for Steam-Link...."
export DESTDIR="${PWD}/output/smw-steamlink"
cd output

# Build TAR.GZ
name=$(basename ${DESTDIR})
tar zcvf $name.tgz $name || exit 3
echo "Build Complete! Check in /output/ directory."

echo "Build Complete!"
