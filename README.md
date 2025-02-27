# steganography

PNG steganography in C with zlib.

> [!Note]
> There are issues with the output of the program.

## Why

I watched a PirateSoftware YouTube Short about how he stores his password using steganography, so I had to try it out.

## Project Status

:white_check_mark: What Works

:construction: In Progress

:bug: Known Issues

:bulb: Future Plans

## Usage

## Building

Requirements

- A C++ compiler
- CMake

```
cmake -B build
```

```
cmake --build build
```

## How

The program uses a technique known as Least Significant Bit(s) (LSB) steganography to hide strings inside PNG images. This works by only modyfing the two least significant bits of each of the bytes of the image's data.