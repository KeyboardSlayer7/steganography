cmake_minimum_required(VERSION 3.27.0)

project(
    Steganography
    VERSION 0.1
    LANGUAGES C
)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
message(CMAKE_RUNTIME_OUTPUT_DIRECTORY: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

set(
    SOURCES
    src/main.c
    src/png.h
    src/png.c
    src/bit_manipulation.h
    src/bit_manipulation.c
    src/cmd.h
    src/cmd.c
    src/compression.h
    src/compression.c
    src/global.h
    src/global.c
    src/filter.h
    src/filter.c
)

# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(
    Steganography
    ${SOURCES}
)

find_package(ZLIB REQUIRED)

target_include_directories(
    Steganography 
    PUBLIC 
    ${ZLIB_INCLUDE_DIRS}
    ${ZLIB_INCLUDE_DIRS}/build
)

# target_link_directories(
#     Steganography
#     PUBLIC
#     ${ZLIB_ROOT_DIR}
# )

target_link_libraries(
    Steganography 
    PUBLIC
    ${ZLIB_LIBRARIES}
)