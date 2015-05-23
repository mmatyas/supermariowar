# Apple specific compiler settings
# --------------------------------

# Use clang compiler
set(CMAKE_C_COMPILER "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang")
set(CMAKE_CXX_COMPILER "/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/clang++")
if (NOT EXISTS "${CMAKE_CXX_COMPILER}")
	set(CMAKE_C_COMPILER "clang")
	set(CMAKE_CXX_COMPILER "clang++")
endif()

# Set up paths
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
	# The gettext module will install the translations unconditionally.
	# Redirect the installation to a build directory where it does no harm.
	set(CMAKE_INSTALL_PREFIX ${CMAKE_BINARY_DIR}/install-dummy)
endif()


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
