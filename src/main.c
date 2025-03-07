#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "zlib.h"
#include "global.h"
#include "png.h"
#include "cmd.h"
#include "filter.h"
#include "bit_manipulation.h"

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

    // for (int i = 0; i < 15; ++i)
    // {
    //     appendint(&arr, i + 1);
    // }

    // for (int i = 0; i < 15; ++i)
    // {
    //     int value = getint(&arr, i);
    //     printf("%d\n", value);
    // }

    // freeArray(&arr);

    // uint8_t value = 68;
    
    // for (int i = 0; i < sizeof(uint32_t); ++i)
    // {
    //     printf("%d: %d\n", i, getNthBitPair(value, i + 1));
    // }

    // int remove_output = remove("out.png");

    // if (remove_output != 0)
    // {
    //     printf("remove() returned %d\n", remove_output);
    // }

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

    if (argv[2][0] == 't')
    {
        const char* message = "Hello!";
        createModifiedPNGFile(&png, message); 
    }
    else if (argv[2][0] == 'f')
    {
        printf("message: %s\n", retrieveFromPNGFile(&png));
    }

    // uint8_t options = parseOptions(argv, argc);

    // switch (options)
    // {
    //     case HIDE_STRING:
    //         createModifiedPNGFile(&png, message); 
    //         break;
    //     case HIDE_FILE:
    //         createModifiedPNGFile(&png, message);
    //         break;
    //     case EXTRACTION:

    //         break;
    //     default:
    //         break;
    // }

    // byte arr[22] = {
    //     0 , 1 , 2 , 3 , 4 , 5 , 6 , 7 , 8 , 9 , 10, 
    //     11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21};
    //     11, 12, 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 , 6 }; 
    // byte arr2[22];
    
    // FilterData data;
    // data.before = arr;
    // data.after = arr2;
    // data.bpp = 1;
    // data.scanline = 11;
    // data.scanline_length = 11;
    // data.apply = false;
    
    // for (int i = 0; i < 11; ++i)
    // {
    //     arr2[i] = arr[i];
    // }
    
    // filterAverage(&data);
    
    // for (int i = 11; i < 22; ++i)
    // {
    //     printf("%d\n", arr2[i]);
    // }
}