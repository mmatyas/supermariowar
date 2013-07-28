smw
====

A multiplayer platformer game.

Supported platforms:

- Linux
- Windows
- XBox
- MAC OS X

How to compile on Linux
=======================
Enter the Build directory, run cmake, then make and finally you can use the binary smw in Build/Binaries directories.

$# cd Build && cmake .. && make -j5
$# Binaries/smw ../

If you have troubles at compiling you might want to use instead:

$# VERBOSE=1 make -j5

To compile with debugging symbols enable the relative option:

$# cd Build && cmake -DDEBUG=ON .. && make -j5
$# gdb Binaries/smw 
gdb> run ../

How to compile on Windows
=========================

You will need CMake and some manual fixes due to the buggy support on Windows.

- get CMake from http://www.cmake.org/cmake/resources/software.html#latest
- install it and select option to have CMake in your PATH
- make sure you have Visual Studio 2010 Express (or better) installed
- get SDL_image (development) from http://www.libsdl.org/projects/SDL_image/
- get SDL_mixer (development) from http://www.libsdl.org/projects/SDL_mixer/
- get SDL_net (development) from http://www.libsdl.org/projects/SDL_net/
- get SDL 1.2 (development) from http://www.libsdl.org/download-1.2.php
- copy all include files in a folder, and all *.lib files in another folder
- manually patch your CMake files in %ProgramFiles%\CMake 2.8\share\cmake-2.8\Modules by using the provided cmake-patches.diff (you can do this manually). Make sure you specify your folder there
- go to Build directory and issue 'cmake ..'
- open the generated solution smw.sln, right-click on the smw project and go to Configuration Properties -> C/C++ - Command line and remove everything there
- on the same group, go to Linker and add your lib directory there
- change also system to WINDOWS (not console)
- now it will compile and smw.exe can be found in build\Binaries\Debug\smw.exe. make sure you copy the necessary SDL dlls along with smw.exe, otherwise it won't work

How to debug on Windows
=======================

In the Debug options group, set the smw data directory as command line argument and the SDL dlls directory as working folder

Compilation options
-------------------

Open CMakeLists.txt to see what options are available, they are described on the first lines of the file.

How to play
===========

Please see documentation in docs/ directory.
