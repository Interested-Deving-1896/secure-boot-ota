
cmake_minimum_required(VERSION 3.10)
project(SecureBootOTA C)

set(CMAKE_C_STANDARD 11)

include_directories(include)

file(GLOB SOURCES
    "src/*.c"
    "src/platform_specific/*.c"
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    USE_HARDWARE_SECURITY=1
    USE_TRUSTZONE=1
)
