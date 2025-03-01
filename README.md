# steganography

PNG steganography in C with zlib.

> [!Note]
> There are issues with the output of the program.

## Why

I watched a PirateSoftware YouTube Short about how he stores his password using steganography, so I had to try it out.

## Project Status

:white_check_mark: What Works

- Embedding strings in PNG images
- Retrieving embedded strings from PNG images

:construction: In Progress

- Steganography with PNGs that use the Paeth filter 
- Embedding files in PNGs

:bug: Known Issues

-  

:bulb: Future Plans

- Maintenance and optimisations 

## Usage

```
Steganography <image>
```

## Building

Requirements

- A C compiler
- CMake

```
cmake -B build
```

```
cmake --build build
```

## How

The program uses a technique known as Least Significant Bit(s) (LSB) steganography to hide strings inside PNG images. This works by only modyfing the two least significant bits of each of the bytes of the image's pixel data. 

## Feedback

I've only been working with C/C++ for a few years and am still fairly new to CMake, so I'd love to hear thoughts! If you find any bugs or have any suggestions, feel free to email me at keyboardslayer7@protonmail.com. Any feedback, big or small, is greatly appreciated!