# ARM specific compiler settings
# ------------------------------

set(ARM_FLAGS "-marm")
if (CMAKE_SYSTEM_PROCESSOR MATCHES "^armv6")
	set(ARM_FLAGS "${ARM_FLAGS} -march=armv6")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^armv7")
	set(ARM_FLAGS "${ARM_FLAGS} -march=armv7-a")
else()
	message("Unknown ARM device detected; if you have troubles, try calling CMake with -DDISABLE_DEFAULT_CFLAGS=1, and set your CFLAGS and CPPFLAGS")
endif()

# Select the optimal FPU settings
# If you get linking errors, try editing these flags
set(ARM_FLAGS "${ARM_FLAGS} -mfpu=vfp -mfloat-abi=hard")

# Apply the flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARM_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARM_FLAGS}")
