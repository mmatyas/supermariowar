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

# Options for customizing builds
option(DEBUG "Compile with debugging information" OFF)
option(USE_PNG_SAVE "Enable PNG save feature" OFF) # currently working only with 1.2 ?
option(USE_EMSCRIPTEN "Output asm.js code using emscripten" OFF) # TODO: change this to a build type maybe?
option(USE_SDL2_LIBS "Use SDL2 instead of SDL 1.x" OFF) # only smw ported yet
option(NO_NETWORK "Disable all network communication" OFF)

# User settings directory
set(userdir ".smw" CACHE STRING "User directory")
add_definitions(-DUSER_DIR="${userdir}")


#-----------------------------------------------------------------------------
#
# Platform-specific settings
#
#-----------------------------------------------------------------------------
# TODO: Review and test this section


# option(UNITTESTS "Build unittests" OFF)

# Update compiler before calling project()
if (APPLE)
	# Use clang compiler
	set(CMAKE_C_COMPILER "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang")
	set(CMAKE_CXX_COMPILER "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++")
	if (NOT EXISTS "${CMAKE_CXX_COMPILER}")
		set(CMAKE_C_COMPILER "clang")
		set(CMAKE_CXX_COMPILER "clang++")
	endif()
endif()

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/CMakeTests)

# Set up paths
if((${CMAKE_SYSTEM_NAME} MATCHES "Darwin"))
	# The gettext module will install the translations unconditionally.
	# Redirect the installation to a build directory where it does no harm.
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install-dummy)
else()
	set(bindir  ${CMAKE_INSTALL_PREFIX}/games/bin       CACHE PATH "bindir")
	## data dir will contain the default pack
	set(datadir ${CMAKE_INSTALL_PREFIX}/share/games/smw CACHE PATH "datadir")
	add_definitions(-DDATA_DIR="${datadir}/")
endif()

# Precompiled header support for MSVC:
# Call this after setting the source list (and don't add the source file used
# to generate the pch file, this will be done here automatically)
function(enable_precompiled_headers PRECOMPILED_HEADER SOURCE_FILE SOURCE_VARIABLE_NAME)
	if(MSVC)
		set(files ${${SOURCE_VARIABLE_NAME}})

		# Generate precompiled header translation unit
		get_filename_component(pch_basename ${PRECOMPILED_HEADER} NAME_WE)
		set(pch_abs ${CMAKE_CURRENT_SOURCE_DIR}/${PRECOMPILED_HEADER})
		set(pch_unity ${CMAKE_CURRENT_SOURCE_DIR}/${SOURCE_FILE})
		set_source_files_properties(${pch_unity}  PROPERTIES COMPILE_FLAGS
			"/Yc\"${pch_abs}\"")

		# Update properties of source files to use the precompiled header.
		# Additionally, force the inclusion of the precompiled header at
		# beginning of each source file.
		foreach(source_file ${files} )
			set_source_files_properties(${source_file} PROPERTIES COMPILE_FLAGS
				"/Yu\"${pch_abs}\" /FI\"${pch_abs}\"")
		endforeach(source_file)

		# Finally, update the source file collection to contain the
		# precompiled header translation unit
		set(${SOURCE_VARIABLE_NAME} ${pch_unity} ${${SOURCE_VARIABLE_NAME}} PARENT_SCOPE)
	endif(MSVC)
endfunction(enable_precompiled_headers)

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "^arm")
	set(_M_GENERIC 1)
	set(_M_ARM 1)
	add_definitions(-marm)

	## NEON optimizations for armv7
	if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "7")
		##TODO: support armel
		add_definitions(-mfpu=neon -mfloat-abi=softfp)
	endif()

	add_definitions(-D_M_ARM=1)
	add_definitions(-D_M_GENERIC=1)
endif()


#-----------------------------------------------------------------------------
#
# Compiler flags
#
#-----------------------------------------------------------------------------

# Debugging CFLAGS. Turn optimizations off; turn debugging symbols on.
if(DEBUG)
  add_definitions(-DDEBUG -D_DEBUG -DPNG_DEBUG=1)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -O0")
else()
  add_definitions(-DNDEBUG)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -s")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O3 -s")
endif(DEBUG)

# Set these next two lines to test generic
#set(_M_GENERIC 1)
#add_definitions(-D_M_GENERIC=1)
# Various compile flags
if(NOT _M_GENERIC AND NOT USE_EMSCRIPTEN)
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

#
# Game-related flags
#

# Set where the binary files will be built.
if (DEBUG)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/Debug)
else()
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/Release)
endif()

if(USE_SDL2_LIBS)
	add_definitions(-DUSE_SDL2)
endif()

if(NO_NETWORK)
	add_definitions(-DNETWORK_DISABLED)
endif()


#-----------------------------------------------------------------------------
#
# Dependencies
#
#-----------------------------------------------------------------------------

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if(WIN32)
#	enable_precompiled_headers(Src/stdafx.h Src/stdafx.cpp SRCS)
endif(WIN32)

# emscripten uses its own SDL, no need to include or link
if(NOT USE_EMSCRIPTEN)
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
	find_package(ENet REQUIRED)
endif()

## the optional libpng package
if(USE_PNG_SAVE)
	include(FindPNG)
	add_definitions(-DPNG_SAVE_FORMAT)
	find_package(PNG REQUIRED)
	include_directories(${PNG_INCLUDE_DIR})
	target_link_libraries(smw ${PNG_LIBRARY})

	LIST(APPEND COMMON_SRCS src/savepng.cpp)
endif()

check_and_add_flag(VISIBILITY_INLINES_HIDDEN -fvisibility-inlines-hidden)

if(UNIX AND NOT APPLE)
	check_and_add_flag(VISIBILITY_HIDDEN -fvisibility=hidden)
endif()

if(APPLE)
	# Ignore MacPorts and Fink and any other locally installed packages that
	# might prevent building a distributable binary.
	set(CMAKE_SYSTEM_PREFIX_PATH /usr)
	set(ENV{PATH} /usr/bin:/bin:/usr/sbin:/sbin)

	# Some of our code contains Objective C constructs.
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -x objective-c -stdlib=libc++")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -x objective-c++ -stdlib=libc++")
	# Avoid mistaking an object file for a source file on the link command line.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -x none")

	# Identify the target system:
	# Ask for 64-bit binary.
	set(TARGET_FLAGS "-arch x86_64")
	# Minimum OS X version.
	# This is inserted into the Info.plist as well.
	# Note that the SDK determines the maximum version of which optional
	# features can be used, not the minimum required version to run.
	set(OSX_MIN_VERSION "10.7")
	set(TARGET_FLAGS "${TARGET_FLAGS} -mmacosx-version-min=${OSX_MIN_VERSION}")
	set(SYSROOT_LEGACY_PATH "/Developer/SDKs/MacOSX10.7.sdk")
	set(SYSROOT_PATH "/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.7.sdk")
	if(EXISTS "${SYSROOT_PATH}/")
        set(TARGET_SYSROOT ${SYSROOT_PATH})
	elseif(EXISTS "${SYSROOT_LEGACY_PATH}/")
        set(TARGET_SYSROOT ${SYSROOT_LEGACY_PATH})
	endif()
	if(${TARGET_SYSROOT})
        set(TARGET_FLAGS "${TARGET_FLAGS} -isysroot ${TARGET_SYSROOT}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-syslibroot,${TARGET_SYSROOT}")
	endif()
	# Do not warn about frameworks that are not available on all architectures.
	# This avoids a warning when linking with QuickTime.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-no_arch_warnings")
	# Specify target CPUs.
	set(TARGET_FLAGS "${TARGET_FLAGS} -mssse3")
	set(TARGET_FLAGS "${TARGET_FLAGS} -march=core2")
	# Target flags apply to both C and C++ compilation.
	# CMake passes these to the compiler on the link command line as well.
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${TARGET_FLAGS}")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TARGET_FLAGS}")

	# Linker flags.
	# Drop unreachable code and data.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-dead_strip,-dead_strip_dylibs")
	# Reserve the minimum size for the zero page.
	# Our JIT requires virtual memory space below 2GB, while the default zero
	# page on x86_64 is 4GB in size.
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-pagezero_size,0x1000")

	if(NOT DISABLE_WX)
		add_definitions(-DUSE_WX -DHAVE_WX)
		set(USE_WX TRUE)
	endif()
	find_library(APPKIT_LIBRARY AppKit)
	find_library(APPSERV_LIBRARY ApplicationServices)
	find_library(ATB_LIBRARY AudioToolbox)
	find_library(AU_LIBRARY AudioUnit)
	find_library(CARBON_LIBRARY Carbon)
	find_library(COCOA_LIBRARY Cocoa)
	find_library(COREAUDIO_LIBRARY CoreAudio)
	find_library(COREFUND_LIBRARY CoreFoundation)
	find_library(CORESERV_LIBRARY CoreServices)
	find_library(IOB_LIBRARY IOBluetooth)
	find_library(IOK_LIBRARY IOKit)
	find_library(QUICKTIME_LIBRARY QuickTime)
	find_library(WEBKIT_LIBRARY WebKit)
endif()

if(WIN32)
	add_definitions(-D_SECURE_SCL=0)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
	add_definitions(-D_CRT_SECURE_NO_DEPRECATE)
endif(WIN32)

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING
		"Build type (Release/Debug/RelWithDebInfo/MinSizeRe)" FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(CMAKE_BUILD_TYPE STREQUAL Release AND NOT APPLE AND NOT WIN32)
	add_definitions(-fomit-frame-pointer)
endif()

##TODO: test this
add_definitions(-D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE)

if(ANDROID)
	message("Building for Android")
	add_definitions(-DANDROID)
endif()

if (APPLE)
	message("Building for Apple")
	add_definitions(-D__MACOSX__)
	##SDL_CFLAGS=`sdl-config --cflags`
	##SDL_LIBS=`sdl-config --libs`
	#SDK="-arch ppc -arch i386 -isysroot /Developer/SDKs/MacOSX10.4u.sdk"
	#SDL_CFLAGS="$SDK -I/Library/Frameworks/SDL.framework/Headers -I/Library/Frameworks/SDL_net.framework/Headers -I/Library/Frameworks/SDL_mixer.framework/Headers -I/Library/Frameworks/SDL_image.framework/Headers"
	#SDL_LIBS="-F/Library/Frameworks -framework CoreFoundation -framework Cocoa -framework SDL"

	## copied over from original Makefile
	#echo 'CFLAGS:=$(CFLAGS) -D__MACOSX__ -DLINUXFUNC -DPREFIXPATH=\"\" '$SDL_CFLAGS $CFLAGS >> configuration
	#echo 'LDFLAGS:=$(LDFLAGS) '$SDL_LIBS $LDFLAGS -framework SDL_image -framework SDL_mixer -framework png >> configuration
	#echo 'COMMON_OBJS:=$(COMMON_OBJS) build/linfunc.o build/SDLMain.o' >> configuration
endif()


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
