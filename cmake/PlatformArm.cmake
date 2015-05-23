# ARM specific compiler settings
# ------------------------------

# If you get compiling or linking errors, try editing this file

if (ARM_OVERRIDE_ARCH)
	set(ARM_TARGET_ARCH "${ARM_OVERRIDE_ARCH}")
else()
	set(ARM_TARGET_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(ARM_FLAGS "-marm")
if (ARM_TARGET_ARCH MATCHES "^armv6")
	set(ARM_FLAGS "${ARM_FLAGS} -march=armv6 -mfpu=vfp -mfloat-abi=hard")
elseif (ARM_TARGET_ARCH MATCHES "^armv7")
	set(ARM_FLAGS "${ARM_FLAGS} -march=armv7-a -mfpu=neon -mfloat-abi=softfp")
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARM_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARM_FLAGS}")
