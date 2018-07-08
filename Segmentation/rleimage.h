#ifndef RLEIMAGE_H
#define RLEIMAGE_H

#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#include "byteimage.h"

#pragma pack(push, 1)
typedef struct RleCell_ {
    int begin;
    int end;
    unsigned int label;
} RleCell;
#pragma pack(pop)

typedef struct RleRow_ {
    RleCell* cells;
    uint16_t numberOfCells;
}RleRow;

typedef struct RleImage_ {
    uint16_t width;
    uint16_t height;
    RleRow* rows;
} RleImage;

int createRleImage(const ByteImage* image, RleImage* rle);
int compressImage(const ByteImage* image, RleImage* rle);
void compressRow(uint8_t* imageRow, uint16_t width, RleRow* rleRow);

void releaseRleImage(RleImage** image);
#endif // RLEIMAGE_H
