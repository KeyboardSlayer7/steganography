#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "zlib.h"
#include "global.h"
#include "png.h"

ARRAY_METHODS(int);
ARRAY_METHODS(Chunk);

int main(int argc, char* argv[])
{
    // FILE* input = fopen(argv[1], "rb");

    // printf("IHDR size: %d\n", sizeof(IHDR));
    // printf("offsetof: %d\n", offsetof(struct ihdr_t, compression_method));

    // fseek(input, 0, SEEK_END);
    // uint32_t length = ftell(input);
    // fseek(input, 0, SEEK_SET);

    // printf("File length: %d\n", length);

    // readPNGFile(input);

    // fseek(input, 0, SEEK_SET);
    // createModifiedPNGFile(input, NULL, 0); 
    
    // fclose(input);

    // Array arr;
    // initArray(&arr, sizeof(int));

    // for (int i = 0; i < 10; ++i)
    // {
    //     appendint(&arr, i + 1);
    // }

    // for (int i = 0; i < 10; ++i)
    // {
    //     int value = getint(&arr, i);
    //     printf("%d\n", value);
    // }

    int remove_output = remove("../out.png");

    if (remove_output != 0)
    {
        printf("remove() returned %d\n", remove_output);
    }

    PNG png;
    loadPNG(&png, argv[1]);

    int width, height;
    getDimensions(&png, &width, &height);

    printf("----------\n");
    printf("%d chunks\n", png.chunks.size);
    printf("Width: %d, Height: %d\n", width, height);
    printf("Byte Per Pixel: %d\n", bytesPerPixel(png.ihdr.color_type, png.ihdr.bit_depth));

    printIHDR(&png.ihdr);

    for (int i = 0; i < png.chunks.size; ++i)
    {
        Chunk chunk = getChunk(&png.chunks, i);
        printf("[%d|%s|%lu]\n", chunk.length, chunk.type, chunk.crc);
    }

    const char* message = "I got the devilchicken on my phone he told me keep edging that shit!";
    createModifiedPNGFile(&png, message);
}