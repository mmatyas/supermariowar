smw
====

A multiplayer platformer game.

Supported platforms:

- Linux
- Windows
- XBox
- MAC OS X

How to compile on Linux
==============
Enter the Build directory, run cmake, then make and finally you can use the binary smw in Build/Binaries directories.

$# cd Build && cmake -DCMAKE_BUILD_TYPE=Release .. && make -j5
$# Binaries/smw ../

If you have troubles at compiling you might want to use instead:

$# VERBOSE=1 make -j5

To compile with debugging symbols:

$# cd Build && cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j5
$# gsb Binaries/smw 
gdb> run ../


Compilation options
-------------------

Open CMakeLists.txt to see what options are available, they are described on the first lines of the file.

How to play
===========

Please see documentation in docs/ directory.
