# AArch64 specific compiler settings
# ----------------------------------

set(ARM64_FLAGS "-march=armv8-a")

# Apply the flags
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${ARM64_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${ARM64_FLAGS}")
