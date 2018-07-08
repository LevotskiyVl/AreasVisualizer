#include "rleimage.h"
#include "errors.h"

int createRleImage(const ByteImage *image, RleImage *rle)
{
    if (image->numberOfChannels > 1) { return SIZES_ERROR; }

    rle->height = image->ROI.height;
    rle->width = image->ROI.width;
    rle->data = (RleRow*) malloc(rle->height * sizeof(RleRow));
    if (rle->data == NULL) {
        return ALLOCATION_ERROR;
    }

    for (size_t i = 0; i < rle->height; i++) {
        rle->data[i] = (RleCell*) calloc((rle->width + 1) / 2, sizeof(RleCell));
        if (rle->data[i] == NULL) {
            for (int j =0; j < i; j++) {
                free(rle->data[i]);
            }
            free(rle->data);
            rle->data = NULL;
            return ALLOCATION_ERROR;
        }
    }

    compressImage(image, rle);
}

int compressImage(const ByteImage* image, RleImage* rle)
{
    if (image == NULL || rle == NULL ||
            rle->data == NULL || image->data == NULL) { return NULL_PTR_ERROR; }

    if (image->ROI.height != rle->height || image->ROI.width != rle->width) {
        return SIZES_ERROR;
    }

    uint8_t* imageData = &image->data[(image->ROI.yOffset * image->widthStep)
                                       + image->ROI.xOffset];

    for (size_t i = 0; i < rle->height; i++) {
        uint8_t* rowPointer = &imageData[i * image->widthStep];
        compressRow(rowPointer, image->ROI.width, rle->data[i]);
    }
}

void compressRow(uint8_t* imageRow, uint16_t width, RleRow* rleRow)
{
    bool compressing = false;
    size_t numberOfCells = 0;
    for (size_t i = 0; i < width; i++) {
        if (imageRow[i]) {
            if (!compressing) {
                rleRow->cells[numberOfCells].begin = i;
                compressing = true;
            }
        }
        else {
            if (compressing) {
                compressing = false;
                rleRow->cells[numberOfCells].end = i - 1;
                numberOfCells++;
            }
        }
    }
    if (compressing) {
        rleRow->cells[numberOfCells].end = width - 1;
        numberOfCells++;
    }
    rleRow->numberOfCells = numberOfCells;
}

void releaseRleImage(RleImage** image)
{
    if (*image == NULL) {
        return;
    }
    if ((*image)->data != NULL) {
        for (int i = 0; i < (*image)->height; i++) {
            free((*image)->data[i]);
        }
    }
    free(*image);
    *image = NULL;
}
