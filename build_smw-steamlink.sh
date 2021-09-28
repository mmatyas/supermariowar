#!/bin/bash
#
echo "Checking For SDK..."
source "../../setenv.sh"
rm -r build
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=../../toolchain/steamlink-toolchain.cmake -DUSE_SDL2_LIBS=1 ..
make smw

# Prepare package directory
rm "./output/smw-steamlink/smw"
rm "./output/smw"

cp "./Binaries/Release/smw" "../output/smw-steamlink/smw"
cp "./Binaries/Release/smw" "../output/smw"

cd ..
rm -r build

echo "Packaging it for Steam-Link...."
export DESTDIR="${PWD}/output/smw-steamlink"
cd output/smw-steamlink

# Build TAR.GZ
ls -1A | xargs -d "\n" tar cvfz SuperMarioWar-SteamLink.tgz
mv SuperMarioWar-SteamLink.tgz ../SuperMarioWar-SteamLink.tgz
echo "Build Complete! Check in /output/ directory."

echo "Build Complete!"
