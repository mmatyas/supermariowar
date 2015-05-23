# ARM specific compiler settings
# ------------------------------

# If you get compiling or linking errors, try editing this file

set(ARM_FLAGS "-marm")
if (CMAKE_SYSTEM_PROCESSOR MATCHES "^armv6")
	set(ARM_FLAGS "${ARM_FLAGS} -march=armv6 -mfpu=vfp -mfloat-abi=hard")
elseif (CMAKE_SYSTEM_PROCESSOR MATCHES "^armv7")
	set(ARM_FLAGS "${ARM_FLAGS} -march=armv7-a -mfpu=neon -mfloat-abi=softfp")
endif()

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARM_FLAGS}")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARM_FLAGS}")
