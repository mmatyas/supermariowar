# ARM specific compiler settings
# ------------------------------

# Overrides CMAKE_SYSTEM_PROCESSOR if set
if (ARM_OVERRIDE_ARCH)
	set(ARM_TARGET_ARCH "${ARM_OVERRIDE_ARCH}")
else()
	set(ARM_TARGET_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
endif()

# Select the optimal FPU settings
# If you get linking errors, try editing these flags
if (ARM_OVERRIDE_FLAGS)
	set(ARM_FLAGS "${ARM_OVERRIDE_FLAGS}")
else()
	set(ARM_FLAGS "-marm")
	if (ARM_TARGET_ARCH MATCHES "^armv6")
		set(ARM_FLAGS "${ARM_FLAGS} -march=armv6")
	elseif (ARM_TARGET_ARCH MATCHES "^armv7")
		set(ARM_FLAGS "${ARM_FLAGS} -march=armv7-a")
	else()
		message("Unknown ARM device detected; if you have troubles, try using ARM_OVERRIDE_FLAGS")
	endif()
	set(ARM_FLAGS "${ARM_FLAGS} -mfpu=vfp -mfloat-abi=hard")
endif()

# Apply the flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARM_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARM_FLAGS}")
