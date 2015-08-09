#! /bin/bash

echo -e "SMW Build test system\n\n"

set -e # exit on error
set -x # show commands

if [ ! -d ./tools ]; then
    echo -e "Please run this tool from the root directory."
    exit 1
fi

rm -rf build-test
mkdir build-test
cd build-test

#
# Test native release build
#

echo -e "###\n\nTest 1: Native release build.\n\n###"

mkdir -p native
cd native

cmake -D CMAKE_BUILD_TYPE:STRING=Release -G "Unix Makefiles" ../.. > /dev/null
make -j2 > /dev/null
cd ..

#
# Test native debug build
#

echo -e "###\n\nTest 2: Native debug build.\n\n###"

mkdir -p native-debug
cd native-debug

cmake -D CMAKE_BUILD_TYPE:STRING=Debug -G "Unix Makefiles" ../.. > /dev/null
make -j2 > /dev/null
cd ..

#
# Test SDL2 (only smw yet)
#

echo -e "###\n\nTest 3: SDL2 release build.\n\n###"

mkdir -p native-sdl2
cd native-sdl2

cmake -D CMAKE_BUILD_TYPE:STRING=Release -D USE_SDL2_LIBS:BOOL=ON -G "Unix Makefiles" ../.. > /dev/null
make -j2 smw > /dev/null
cd ..

#
# Test disabled networking
#

echo -e "###\n\nTest 4: Release build without networking.\n\n###"

mkdir -p native-nullnet
cd native-nullnet

cmake -D CMAKE_BUILD_TYPE:STRING=Release -D NO_NETWORK:BOOL=ON -G "Unix Makefiles" ../.. > /dev/null
make -j2 > /dev/null
cd ..

#
# Test JavaScript release build
#

echo -e "###\n\nTest 5: JavaScript/asm.js release build.\n\n###"

mkdir -p js
cd js

ln -s ../../data data
emconfigure cmake \
    -D CMAKE_BUILD_TYPE:STRING=Release \
    -D TARGET_EMSCRIPTEN:BOOL=ON \
    -D NO_NETWORK:BOOL=ON \
    -G "Unix Makefiles" ../.. > /dev/null
emmake make -j2 > /dev/null

EMCC_PARAMS="-O3 -v --preload-file data -s OUTLINING_LIMIT=60000 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1"
emcc Binaries/smw.bc -o smw.html $EMCC_PARAMS > /dev/null
emcc Binaries/leveledit.bc -o leveledit.html $EMCC_PARAMS > /dev/null
emcc Binaries/worldedit.bc -o worldedit.html $EMCC_PARAMS > /dev/null
cd ..

#
# Test JavaScript debug build
#

echo -e "###\n\nTest 6: JavaScript/asm.js debug build.\n\n###"

mkdir -p js-debug
cd js-debug

ln -s ../../data data
emconfigure cmake \
    -D CMAKE_BUILD_TYPE:STRING=Debug \
    -D TARGET_EMSCRIPTEN:BOOL=ON \
    -D NO_NETWORK:BOOL=ON \
    -G "Unix Makefiles" ../.. > /dev/null
emmake make -j2 > /dev/null

EMCC_PARAMS="-O0 -v --preload-file data -s OUTLINING_LIMIT=60000 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 -s SAFE_HEAP=1 -s ALIASING_FUNCTION_POINTERS=0 --js-opts 0 -g4"
emcc Binaries/smw.bc -o smw.html $EMCC_PARAMS > /dev/null
emcc Binaries/leveledit.bc -o leveledit.html $EMCC_PARAMS > /dev/null
emcc Binaries/worldedit.bc -o worldedit.html $EMCC_PARAMS > /dev/null
cd ..

#
# End
#

echo -e "###\n\nTests completed successfully."
cd ..
