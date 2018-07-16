#include "rleimage.h"
#include "Recognizer/Recognizer_errors.h"

int createRleImage(const ByteImage *image, RleImage *rle)
{
    if (image->numberOfChannels > 1) { return SIZES_ERROR; }

    rle->height = image->ROI.height;
    rle->width = image->ROI.width;
    rle->rows = (RleRow*) malloc(rle->height * sizeof(RleRow));
    if (rle->rows == NULL) {
        return ALLOCATION_ERROR;
    }

    for (size_t i = 0; i < rle->height; i++) {
        rle->rows[i].cells = (RleCell*) calloc((rle->width + 1) / 2, sizeof(RleCell));
        if (rle->rows[i].cells == NULL) {
            for (size_t j =0; j < i; j++) {
                free(rle->rows[i].cells);
            }
            free(rle->rows);
            rle->rows = NULL;
            return ALLOCATION_ERROR;
        }
    }

    compressImage(image, rle);
    return SUCCESS;
}

int compressImage(const ByteImage* image, RleImage* rle)
{
    if (image == NULL || rle == NULL ||
            rle->rows == NULL || image->data == NULL) { return NULL_PTR_ERROR; }

    if (image->ROI.height != rle->height || image->ROI.width != rle->width) {
        return SIZES_ERROR;
    }

    uint8_t* imageData = &image->data[(image->ROI.yOffset * image->widthStep)
                                       + image->ROI.xOffset];

    for (size_t i = 0; i < rle->height; i++) {
        uint8_t* rowPointer = &imageData[i * image->widthStep];
        compressRow(rowPointer, image->ROI.width, &rle->rows[i]);
    }
    return SUCCESS;
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

void releaseRleImage(RleImage* image)
{
    if (image == NULL) {
        return;
    }
    if (image->rows != NULL) {
        for (int i = 0; i < image->height; i++) {
            free(image->rows[i].cells);
        }
    }
    free(image);
}
