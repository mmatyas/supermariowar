#-----------------------------------------------------------------------------
#
# Super Mario War project configuration file
#
#-----------------------------------------------------------------------------

cmake_minimum_required(VERSION 2.6)

project(smw)

set(SMW_VERSION_MAJOR "2")
set(SMW_VERSION_MINOR "0")
set(SMW_VERSION_PATCH "0")

#
# Options for customizing builds
#

option(USE_PNG_SAVE "Enable PNG save feature" OFF) # currently working only with 1.2 ?
option(TARGET_EMSCRIPTEN "Output asm.js code using emscripten" OFF) # TODO: change this to a build type maybe?
option(USE_SDL2_LIBS "Use SDL2 instead of SDL 1.x" OFF) # only smw ported yet
option(NO_NETWORK "Disable all network communication" OFF)

if(USE_SDL2_LIBS)
	add_definitions(-DUSE_SDL2)
endif()

if(NO_NETWORK)
	add_definitions(-DNETWORK_DISABLED)
endif()

if(USE_PNG_SAVE)
	add_definitions(-DPNG_SAVE_FORMAT)
endif()

# option(UNITTESTS "Build unittests" OFF)
# set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeTests)

# Additional CMake search path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING
		"Build type (Release/Debug/RelWithDebInfo/MinSizeRe)" FORCE)
endif()

#
# Game-related flags
#

# Set where the binary files will be built.
if (CMAKE_BUILD_TYPE STREQUAL Debug)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/Debug)
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/Release)
endif()


#-----------------------------------------------------------------------------
#
# Platform-specific settings
#
#-----------------------------------------------------------------------------

if(APPLE)
	include(PlatformApple)
endif()

if (MSVC)
	include(PlatformMSVC)
endif()

if (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
	set(CPU_ARM 1)
	set(CPU_ARM32 1)
	include(PlatformArm)
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
	set(CPU_ARM 1)
	set(CPU_ARM64 1)
	include(PlatformArm64)
endif()

if(ANDROID)
	message("Building for Android")
	add_definitions(-DANDROID)
endif()

#
# Install locations
#

set(userdir ".smw" CACHE STRING "User directory") # User settings directory
set(datadir ${CMAKE_INSTALL_PREFIX}/share/games/smw CACHE PATH "datadir") # data dir will contain the default pack
set(bindir  ${CMAKE_INSTALL_PREFIX}/games/bin       CACHE PATH "bindir")


#-----------------------------------------------------------------------------
#
# Compiler flags
#
#-----------------------------------------------------------------------------

# Debugging CFLAGS. Turn optimizations off; turn debugging symbols on.
if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-DDEBUG -D_DEBUG -DPNG_DEBUG=1)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -O0")
else()
  add_definitions(-DNDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -s")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O3 -s")
endif()

# Set these next two lines to test generic
#set(CPU_ARM 1)
#add_definitions(-DCPU_ARM=1)
# Various compile flags
if(NOT CPU_ARM AND NOT TARGET_EMSCRIPTEN)
	add_definitions(-msse2)
endif()

include(CheckCXXCompilerFlag)
macro(check_and_add_flag var flag)
	CHECK_CXX_COMPILER_FLAG(${flag} FLAG_${var})
	if(FLAG_${var})
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}")
	endif()
endmacro()

# Enabling all warnings in MSVC spams too much
if(NOT MSVC)
#	add_definitions(-Wall)

	# TODO: would like these but they produce overwhelming amounts of warnings
	#check_and_add_flag(EXTRA -Wextra)
	#check_and_add_flag(MISSING_FIELD_INITIALIZERS -Wmissing-field-initializers)
	#check_and_add_flag(SWITCH_DEFAULT -Wswitch-default)
	#check_and_add_flag(FLOAT_EQUAL -Wfloat-equal)
	#check_and_add_flag(CONVERSION -Wconversion)
	#check_and_add_flag(ZERO_AS_NULL_POINTER_CONSTANT -Wzero-as-null-pointer-constant)
	check_and_add_flag(TYPE_LIMITS -Wtype-limits)
	check_and_add_flag(SIGN_COMPARE -Wsign-compare)
	check_and_add_flag(IGNORED_QUALIFIERS -Wignored-qualifiers)
	check_and_add_flag(UNINITIALIZED -Wuninitialized)
	check_and_add_flag(LOGICAL_OP -Wlogical-op)
	check_and_add_flag(SHADOW -Wshadow)
	check_and_add_flag(INIT_SELF -Winit-self)

	# gcc uses some optimizations which might break stuff without this flag
	add_definitions(-fno-strict-aliasing -fexceptions -Wno-shadow)

endif(NOT MSVC)

##TODO: test this please, coming from smw Makefile
add_definitions(-D_GNU_SOURCE=1 -D_REENTRANT)

## MAIN CONFIG

##TODO: clean code to not need this anymore
#add_definitions(-Wno-shadow)
#add_definitions(-std=gnu++0x)

check_and_add_flag(VISIBILITY_INLINES_HIDDEN -fvisibility-inlines-hidden)

if(UNIX AND NOT APPLE)
	check_and_add_flag(VISIBILITY_HIDDEN -fvisibility=hidden)
endif()

if(CMAKE_BUILD_TYPE STREQUAL Release AND UNIX AND NOT APPLE)
	add_definitions(-fomit-frame-pointer)
endif()

##TODO: test this
add_definitions(-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE)


#-----------------------------------------------------------------------------
#
# Dependencies
#
#-----------------------------------------------------------------------------

# emscripten uses its own SDL, no need to include or link
if(NOT TARGET_EMSCRIPTEN)
	if(USE_SDL2_LIBS)
		# add all SDL2 dependencies
		find_package(SDL2 REQUIRED)
		find_package(SDL2_mixer REQUIRED)
		find_package(SDL2_image REQUIRED)

		# location of SDL headers
		include_directories(${SDL2_INCLUDE_DIR})
		include_directories(${SDL2_IMAGE_INCLUDE_DIRS})
		include_directories(${SDL2_MIXER_INCLUDE_DIRS})
	else()
		# add all SDL dependencies
		find_package(SDL REQUIRED)
		find_package(SDL_mixer REQUIRED)
		find_package(SDL_image REQUIRED)

		# location of SDL headers
		include_directories(${SDL_INCLUDE_DIR})
		include_directories(${SDL_IMAGE_INCLUDE_DIRS})
		include_directories(${SDL_MIXER_INCLUDE_DIRS})
	endif()
endif()

if(NOT NO_NETWORK)
	include_directories(dependencies/enet/include)
	add_subdirectory(dependencies/enet)
endif()

# the optional libpng package
if(USE_PNG_SAVE)
	find_package(PNG REQUIRED)

	include_directories(${PNG_INCLUDE_DIR})
	target_link_libraries(smw ${PNG_LIBRARY})
endif()

# Build custom dependencies
include_directories(dependencies/yaml-cpp-noboost/include)
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "YAML: Build testing and parse tools" FORCE)
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "YAML: Build contrib stuff in library" FORCE)
add_subdirectory(dependencies/yaml-cpp-noboost)

#-----------------------------------------------------------------------------
#
# Ready to build!
#
#-----------------------------------------------------------------------------

# Dump some information
message("    SYSTEM:      ${CMAKE_SYSTEM_NAME} ${CMAKE_SYSTEM_PROCESSOR}")
message("    BUILD:       ${CMAKE_BUILD_TYPE}")
message("    CC:          ${CMAKE_C_COMPILER}")
message("    CXX:         ${CMAKE_CXX_COMPILER}")
message("    C_FLAGS:     ${CMAKE_C_FLAGS}")
message("    CXX_FLAGS:   ${CMAKE_CXX_FLAGS}")

#
# Go through every sub-project
#

include_directories(src/common)
include_directories(src/smw)

add_subdirectory(src/common)
add_subdirectory(src/smw)
add_subdirectory(src/leveleditor)
add_subdirectory(src/worldeditor)
add_subdirectory(src/server)

if(USE_PNG_SAVE)
	LIST(APPEND COMMON_SRCS src/savepng.cpp)
endif()

#-----------------------------------------------------------------------------
#
# Installation and packaging
#
#-----------------------------------------------------------------------------

#
# Data directory install location
#

if((NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin"))
	install(DIRECTORY data DESTINATION ${datadir})
endif()

if((NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux|FreeBSD|Darwin"))
#	install(FILES Data/license.txt DESTINATION ${datadir})
endif()

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	# Install the application icon and menu item
	##TODO: restore these
#	install(FILES src/cpack/smw.xpm DESTINATION ${CMAKE_INSTALL_PREFIX}/share/pixmaps RENAME smw.xpm)
#	install(FILES src/cpack/smw.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications)
endif()

#
# Package settings
#

set(CPACK_PACKAGE_NAME "smw")
set(CPACK_PACKAGE_VENDOR "72dpiarmy")
set(CPACK_PACKAGE_VERSION_MAJOR ${SMW_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SMW_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${SMW_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/src/resources/package_description.txt)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Super Mario War, arcade platform multiplayer game")

set(CPACK_SET_DESTDIR ON)
set(CPACK_SOURCE_IGNORE_FILES  "\\\\.#;/#;.*~;\\\\.swp;/\\\\.git")
list(APPEND CPACK_SOURCE_IGNORE_FILES "${CMAKE_BINARY_DIR}")

set(CPACK_GENERATOR "TGZ;DEB;RPM")
set(CPACK_SOURCE_GENERATOR "TGZ;TBZ2;ZIP")

#
# Debian
#
set(CPACK_DEBIAN_PACKAGE_SECTION Games)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "nobody")
if(USE_SDL2_LIBS)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl2-2.0-0, libsdl2-mixer-2.0-0, libsdl2-image-2.0-0")
else()
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "libsdl1.2debian, libsdl-mixer1.2, libsdl-image1.2")
endif()
if(NOT NO_NETWORK)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libenet2a")
endif()

#
# RPM
#
set(CPACK_RPM_PACKAGE_GROUP Amusements/Games)
set(CPACK_RPM_PACKAGE_LICENSE GPL-2.0)

if(USE_SDL2_LIBS)
	set(CPACK_RPM_PACKAGE_REQUIRES "SDL2, SDL2_image, SDL2_mixer")
else()
	set(CPACK_RPM_PACKAGE_REQUIRES "SDL, SDL_image, SDL_mixer")
endif()
if(NOT NO_NETWORK)
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, enet")
endif()

#
# OSX, Windows: TODO
#
set(CPACK_RESOURCE_FILE_README ${PROJECT_SOURCE_DIR}/README.md)
# TODO: CPACK_RESOURCE_FILE_WELCOME
set(CPACK_PACKAGE_ICON ${PROJECT_SOURCE_DIR}/src/resources/smw.ico)
# TODO: CPACK_NSIS_*

# CPack must be included after the CPACK_* variables are set in order for those
# variables to take effect.
include(CPack)
