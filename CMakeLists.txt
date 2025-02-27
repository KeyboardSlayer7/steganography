cmake_minimum_required(VERSION 3.27.0)

project(
    Steganography
    VERSION 0.1
    LANGUAGES C
)

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

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(
    Steganography
    ${SOURCES}
)

target_include_directories(
    Steganography 
    PUBLIC 
    zlib
)

target_link_directories(
    Steganography
    PUBLIC
    zlib
)

target_link_libraries(
    Steganography 
    zlib
)