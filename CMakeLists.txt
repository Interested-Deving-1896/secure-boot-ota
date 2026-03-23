cmake_minimum_required(VERSION 3.14)
project(secure_boot_ota_demo VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Main executable
add_executable(${PROJECT_NAME}
    src/main.c
    src/secure_boot

