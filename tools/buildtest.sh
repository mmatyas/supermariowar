#! /bin/bash

echo "SMW Build test system"

if [ ! -d ./tools ]; then
	echo "Please run this tool from the root directory."
	exit 1
fi

#rm -rf build-test && mkdir build-test
if [ $? -gt 0 ]; then
    echo "Error: Could not create build-test directory."
    exit 1
fi

cd build-test
if [ $? -gt 0 ]; then
    echo "Error: build-test directory not exists."
    exit 1
fi

#
# Test native release build
# 

echo "Test 1: Native release build."

mkdir -p native && cd native
if [ $? -gt 0 ]; then
    echo "Test failed: Could not create build directory."
    exit 1
fi

cmake -D CMAKE_BUILD_TYPE:STRING=Release -D DEBUG:BOOL=OFF -G "Unix Makefiles" ../..
if [ $? -gt 0 ]; then
    echo "Test failed: Configure error."
    exit 1
fi

make -j2 && cd ..
if [ $? -gt 0 ]; then
    echo "Test failed: Build error."
    exit 1
fi


#
# Test native debug build
# 

echo "Test 2: Native debug build."

mkdir -p native-debug && cd native-debug
if [ $? -gt 0 ]; then
    echo "Test failed: Could not create build directory."
    exit 1
fi

cmake -D CMAKE_BUILD_TYPE:STRING=Debug -D DEBUG:BOOL=ON -G "Unix Makefiles" ../..
if [ $? -gt 0 ]; then
    echo "Test failed: Configure error."
    exit 1
fi

make -j2 && cd ..
if [ $? -gt 0 ]; then
    echo "Test failed: Build error."
    exit 1
fi


#
# Test SDL2 (only smw yet)
# 

echo "Test 3: SDL2 release build."

mkdir -p native-sdl2 && cd native-sdl2
if [ $? -gt 0 ]; then
    echo "Test failed: Could not create build directory."
    exit 1
fi

cmake -D CMAKE_BUILD_TYPE:STRING=Release -D USE_SDL2_LIBS:BOOL=ON -G "Unix Makefiles" ../..
if [ $? -gt 0 ]; then
    echo "Test failed: Configure error."
    exit 1
fi

make -j2 smw && cd ..
if [ $? -gt 0 ]; then
    echo "Test failed: Build error."
    exit 1
fi


#
# Test disabled networking
# 

echo "Test 4: Release build without networking."

mkdir -p native-nullnet && cd native-nullnet
if [ $? -gt 0 ]; then
    echo "Test failed: Could not create build directory."
    exit 1
fi

cmake -D CMAKE_BUILD_TYPE:STRING=Release -D NO_NETWORK:BOOL=ON -G "Unix Makefiles" ../..
if [ $? -gt 0 ]; then
    echo "Test failed: Configure error."
    exit 1
fi

make -j2 && cd ..
if [ $? -gt 0 ]; then
    echo "Test failed: Build error."
    exit 1
fi


#
# Test JavaScript release build
# 

echo "Test 5: JavaScript/asm.js release build."

mkdir -p js && cd js
if [ $? -gt 0 ]; then
    echo "Test failed: Could not create build directory."
    exit 1
fi

if [ ! -d data ]; then
	ln -s ../../data data
	if [ $? -gt 0 ]; then
	    echo "Test failed: Could not create build directory."
	    exit 1
	fi
fi

emconfigure cmake -D CMAKE_BUILD_TYPE:STRING=Release -D DEBUG:BOOL=ON -D USE_EMSCRIPTEN:BOOL=ON -G "Unix Makefiles" ../..
if [ $? -gt 0 ]; then
    echo "Test failed: Configure error."
    exit 1
fi

emmake make -j2
if [ $? -gt 0 ]; then
    echo "Test failed: Build error."
    exit 1
fi

EMCC_PARAMS="-O3 -v --preload-file data -s OUTLINING_LIMIT=60000 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1"
emcc Binaries/smw.bc -o smw.html $EMCC_PARAMS && emcc Binaries/leveledit.bc -o leveledit.html $EMCC_PARAMS && emcc Binaries/worldedit.bc -o worldedit.html $EMCC_PARAMS
if [ $? -gt 0 ]; then
    echo "Test failed: Build error."
    exit 1
fi

cd ..


#
# Test JavaScript debug build
# 

echo "Test 6: JavaScript/asm.js debug build."

mkdir -p js-debug && cd js-debug
if [ $? -gt 0 ]; then
    echo "Test failed: Could not create build directory."
    exit 1
fi

if [ ! -d data ]; then
	ln -s ../../data data
	if [ $? -gt 0 ]; then
	    echo "Test failed: Could not create build directory."
	    exit 1
	fi
fi

emconfigure cmake -D CMAKE_BUILD_TYPE:STRING=Debug -D USE_EMSCRIPTEN:BOOL=ON -G "Unix Makefiles" ../..
if [ $? -gt 0 ]; then
    echo "Test failed: Configure error."
    exit 1
fi

emmake make -j2
if [ $? -gt 0 ]; then
    echo "Test failed: Build error."
    exit 1
fi

EMCC_PARAMS="-O0 -v --preload-file data -s OUTLINING_LIMIT=60000 -s ALLOW_MEMORY_GROWTH=1 -s ASSERTIONS=1 -s SAFE_HEAP=1 -s ALIASING_FUNCTION_POINTERS=0 --js-opts 0 -g4"
emcc Binaries/smw.bc -o smw.html $EMCC_PARAMS && emcc Binaries/leveledit.bc -o leveledit.html $EMCC_PARAMS && emcc Binaries/worldedit.bc -o worldedit.html $EMCC_PARAMS
if [ $? -gt 0 ]; then
    echo "Test failed: Build error."
    exit 1
fi

cd ..


echo "Test completed successfully."
cd ..
