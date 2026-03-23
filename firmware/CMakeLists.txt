
cmake_minimum_required(VERSION 3.16)
project(secure-embedded VERSION 1.0.0 LANGUAGES C ASM)

# Compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Security flags
add_compile_options(
    -Wall -Wextra -Wpedantic
    -fstack-protector-strong
    -D_FORTIFY_SOURCE=2
    -fPIE -pie
)

# Include directories
include_directories(
    ${PROJECT_SOURCE_DIR}/firmware/include
)

# Source files
file(GLOB_RECURSE SOURCES
    "${PROJECT_SOURCE_DIR}/firmware/src/*.c"
    "${PROJECT_SOURCE_DIR}/firmware/src/*.S"
)

# Executable
add_executable(firmware ${SOURCES})

# Link libraries
target_link_libraries(firmware
    mbedTLS
    cmsis
)

# Testing
option(ENABLE_TESTS "Enable unit tests" OFF)
if(ENABLE_TESTS)
    enable_testing()
    add_subdirectory(firmware/tests)
endif()
