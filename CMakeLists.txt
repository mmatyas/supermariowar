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

option(USE_PNG_SAVE "Use PNG for screenshots and thumbnails" OFF)
option(NO_NETWORK "Disable all network communication" OFF)
option(BUILD_STATIC_LIBS "Build and link minor dependencies statically (enet, yaml-cpp)" ON)
option(USE_SDL2_LIBS "Use SDL2 instead of SDL 1.x" OFF) # only smw ported yet
option(SDL2_FORCE_GLES "(SDL2) Use OpenGL ES rendering if possible" OFF)

if (USE_SDL2_LIBS)
	add_definitions(-DUSE_SDL2)

	if (SDL2_FORCE_GLES)
		add_definitions(-DSDL2_FORCE_GLES)
	endif()
endif()

if (NO_NETWORK)
	add_definitions(-DNETWORK_DISABLED)
endif()

if (USE_PNG_SAVE)
	add_definitions(-DPNG_SAVE_FORMAT)
endif()

# Additional CMake search path
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

if (NOT CMAKE_BUILD_TYPE)
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

if (NOT DISABLE_DEFAULT_CFLAGS)
	if (CMAKE_SYSTEM_PROCESSOR MATCHES "(x64|x86_64|amd64)")
		include(PlatformX64)
	elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "(i[3-6]86|x86)")
		include(PlatformX86)
	elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^arm")
		include(PlatformArm)
	elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^aarch64")
		include(PlatformArm64)
	endif()

	if (EMSCRIPTEN)
		include(PlatformEmscripten)
	endif()

	#if (APPLE)
	#	include(PlatformApple)
	#endif()

	if (MSVC)
		include(PlatformMSVC)
	endif()
endif()

#-----------------------------------------------------------------------------
#
# Dependencies
#
#-----------------------------------------------------------------------------

if (NOT DISABLE_SYSLIB_CHECKS)
	find_package(ZLIB REQUIRED)
	include_directories(${ZLIB_INCLUDE_DIR})

	if (USE_SDL2_LIBS)
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

		# the optional libpng package
		if(USE_PNG_SAVE)
			find_package(PNG REQUIRED)
			include_directories(${PNG_INCLUDE_DIR})
		endif()
	endif()
endif()

if (BUILD_STATIC_LIBS)
	macro(check_submodule_git dir)
		if (NOT EXISTS "${CMAKE_SOURCE_DIR}/${dir}/.git")
			message(SEND_ERROR
			"It seems you've forgot to clone the dependencies; ${dir} is not a git repository."
			"Please run `git submodule update --init` first.")
		endif()
	endmacro()

	# Use ENet for networking
	if(NOT NO_NETWORK)
		check_submodule_git(dependencies/enet)
		include_directories(dependencies/enet/include)
		add_subdirectory(dependencies/enet)
	endif()

	# Use YAML for game config files
	check_submodule_git(dependencies/yaml-cpp-noboost)
	include_directories(dependencies/yaml-cpp-noboost/include)
	set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "YAML: Build testing and parse tools" FORCE)
	set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "YAML: Build contrib stuff in library" FORCE)
	add_subdirectory(dependencies/yaml-cpp-noboost)
else()
	find_package(ENet REQUIRED)
	find_package(yaml-cpp REQUIRED)
endif()

#-----------------------------------------------------------------------------
#
# Compiler flags
#
#-----------------------------------------------------------------------------

# Debugging CFLAGS. Turn optimizations off; turn debugging symbols on.
if (CMAKE_BUILD_TYPE STREQUAL Debug)
	add_definitions(-DDEBUG -D_DEBUG -DPNG_DEBUG=1)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -g -O0")
else()
	add_definitions(-DNDEBUG)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c99 -O3")

	# Strip binaries
	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -s")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -s")
	endif()
endif()

# Use C++11
if (WIN32 AND MINGW)
	# MinGW forces strict-ANSI mode with std=c++11
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()

# Link libc statically on Windows to not need additional DLLs
if (WIN32)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libgcc")
	# TODO: check if compiler supports this flag, may fail with GCC < 4.8
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++")
endif()

if (EMSCRIPTEN)
	if (CMAKE_BUILD_TYPE STREQUAL Debug)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g4")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g4")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O0 -g4")
	else()
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -O3")
	endif()
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
	check_and_add_flag(COMPAT_NARROWING_OFF -Wno-narrowing)

	# gcc uses some optimizations which might break stuff without this flag
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-strict-aliasing -fexceptions -Wno-shadow")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fno-strict-aliasing -fexceptions -Wno-shadow")

endif(NOT MSVC)

if (APPLE)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++11-narrowing")
endif()

## MAIN CONFIG

check_and_add_flag(VISIBILITY_INLINES_HIDDEN -fvisibility-inlines-hidden)

if(UNIX AND NOT APPLE)
	check_and_add_flag(VISIBILITY_HIDDEN -fvisibility=hidden)
endif()

#-----------------------------------------------------------------------------
#
# Installation locations
#
#-----------------------------------------------------------------------------

# Common install locations, also passed to the game components

if (WIN32)
	# CMAKE_INSTALL_PREFIX defaults to `C:/Program Files/smw`
	set(SMW_BINDIR  ${CMAKE_INSTALL_PREFIX}      CACHE PATH "Installation location of the game binaries")
	set(SMW_DATADIR ${CMAKE_INSTALL_PREFIX}/data CACHE PATH "Installation location of the game data")
else()
	# CMAKE_INSTALL_PREFIX defaults to `/usr/local`
	set(SMW_BINDIR  ${CMAKE_INSTALL_PREFIX}/games           CACHE PATH "Installation location of the game binaries")
	set(SMW_DATADIR ${CMAKE_INSTALL_PREFIX}/share/games/smw CACHE PATH "Installation location of the game data")
endif()
# set(SMW_USERDIR ".smw" CACHE STRING "User directory") # User settings directory
add_definitions(-DSMW_DATADIR="${SMW_DATADIR}")

# For installing Linux desktop items
if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	if (NOT XDG_APPS_INSTALL_DIR)
		set(XDG_APPS_INSTALL_DIR "/usr/share/applications/")
	endif()
endif()

#
# Data directory install location
#

if((NOT ${CMAKE_SYSTEM_NAME} MATCHES "Darwin"))
	install(DIRECTORY data/ DESTINATION ${SMW_DATADIR})
endif()

if((NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux|FreeBSD|Darwin"))
#	install(FILES Data/license.txt DESTINATION ${datadir})
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
message("    C FLAGS:     ${CMAKE_C_FLAGS}")
message("    CXX FLAGS:   ${CMAKE_CXX_FLAGS}")
message("    LD FLAGS:    ${CMAKE_EXE_LINKER_FLAGS}")

get_directory_property(MSG_DEFINITIONS COMPILE_DEFINITIONS)
message("    DEFINITIONS: ${MSG_DEFINITIONS}")

#
# Go through every sub-project
#

include_directories(src/common)
include_directories(src/common_netplay)
include_directories(src/smw)

add_subdirectory(src/common)
add_subdirectory(src/common_netplay)
add_subdirectory(src/smw)
add_subdirectory(src/leveleditor)
add_subdirectory(src/worldeditor)
add_subdirectory(src/server)

#-----------------------------------------------------------------------------
#
# Packaging
#
#-----------------------------------------------------------------------------

#
# Common settings
#

set(CPACK_PACKAGE_NAME "smw")
set(CPACK_PACKAGE_VENDOR "72dpiarmy")
set(CPACK_PACKAGE_VERSION_MAJOR ${SMW_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${SMW_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${SMW_VERSION_PATCH})
set(CPACK_PACKAGE_DESCRIPTION_FILE ${PROJECT_SOURCE_DIR}/resources/package_description.txt)
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
if (NOT BUILD_STATIC_LIBS)
	if(NOT NO_NETWORK)
		set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libenet2a")
	endif()
	set(CPACK_DEBIAN_PACKAGE_DEPENDS "${CPACK_DEBIAN_PACKAGE_DEPENDS}, libyaml-cpp0.5")
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
if (NOT BUILD_STATIC_LIBS)
	if(NOT NO_NETWORK)
		set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, enet")
	endif()
	set(CPACK_RPM_PACKAGE_REQUIRES "${CPACK_RPM_PACKAGE_REQUIRES}, yaml-cpp")
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
