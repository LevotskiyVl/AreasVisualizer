#include "transofrms.h"
#include "errors.h"
#include <cstring>

int reduceImage(const ByteImage* src, uint8_t reduceStepX, uint8_t reduceStepY, ByteImage* dst)
{
    if (reduceStepX >= src->width || reduceStepX == 0 ||
        reduceStepY >= src->height || reduceStepY == 0) { return SIZES_ERROR; }
    if (src == NULL || dst == NULL) { return NULL_PTR_ERROR; }

    uint16_t width = src->ROI.width;
    uint16_t height = src->ROI.height;
    uint16_t x = src->ROI.xOffset;
    uint16_t y = src->ROI.yOffset;

    size_t reducedWidth = width / reduceStepX;
    size_t reducedHeight = height / reduceStepY;

    for (size_t i = 0; i < reducedHeight; i++) {
        uint8_t* processingRowPointer = &src->data[((i + y) * reduceStepY + x) * src->widthStep];
        uint8_t* dstRowPointer = &dst->data[i * dst->widthStep];
        for (uint16_t j = 0; j < reducedWidth; j++) {
            for (uint16_t c = 0; c < src->numberOfChannels; c++) {
                dstRowPointer[j * src->numberOfChannels + c] = processingRowPointer[j * src->numberOfChannels * reduceStepX + c];
            }
        }
    }
    return SUCCESS;
}

int verticalFlip(ByteImage* image)
{
    if (image == NULL) { return NULL_PTR_ERROR; }
    uint8_t* data = image->data;
    uint16_t width = image->ROI.width;
    uint16_t height = image->ROI.height;
    data += image->ROI.yOffset * image->widthStep
            + image->ROI.xOffset * image->numberOfChannels;

    uint8_t* tempRows = (uint8_t*) calloc(width, sizeof(uint8_t));
    if (tempRows == NULL) {
        return ALLOCATION_ERROR;
    }

    for (int i = 0; i < (height + 1)/ 2; i++) {
        memcpy(tempRows, &data[i * image->widthStep], width * image->numberOfChannels * sizeof(uint8_t));
        memcpy(&data[i * image->widthStep], &data[(height - i - 1) * image->widthStep], width * image->numberOfChannels * sizeof(uint8_t));
        memcpy(&data[(height - i - 1) * image->widthStep], tempRows, width * image->numberOfChannels * sizeof(uint8_t));
    }
    free(tempRows);
    return 0;
}

int blur(const ByteImage* src, ByteImage* dst)
{
    if (src == NULL || dst == NULL) return NULL_PTR_ERROR;
    const double kernel[] = {        0, 0.125,     0,
                                 0.125,  0.25, 0.125,
                                     0, 0.125,     0};
    return convolution(src, kernel, 3, 3, dst);
}

int convolution(const ByteImage* src, const double* kernel, size_t kernelWidth,
                size_t kernelHeight, ByteImage* dst)
{
    if (kernel == NULL) { return NULL_PTR_ERROR;}
    if (src == NULL || dst == NULL) { return NULL_PTR_ERROR;}

    uint8_t anchorX = kernelWidth / 2;
    uint8_t anchorY = kernelHeight / 2;

    uint16_t width = src->ROI.width;
    uint16_t height = src->ROI.height;
    uint16_t x = src->ROI.xOffset;
    uint16_t y = src->ROI.yOffset;
    uint16_t widthStep = src->widthStep;
    uint8_t* data = &src->data[y * widthStep + x];

    for (uint16_t anchYcoord = anchorY; anchYcoord <  height - anchorY; anchYcoord++) {
        uint32_t startHPosition = (anchYcoord - anchorY) * widthStep;
        uint32_t anchorHPosition = anchYcoord * widthStep;

        for (uint32_t anchorXcoord = anchorX; anchorXcoord < (uint16_t)(width - anchorX); anchorXcoord++) {
            uint32_t anchorPosition = anchorHPosition + anchorXcoord;
            double sum = 0;
            uint32_t startWPositoin = (anchorXcoord - anchorX);

            for (uint32_t i = 0; i < kernelHeight; i++) {
                size_t kernelHCoord = i * kernelWidth;
                size_t imgHCoord = startHPosition + (i * widthStep);

                for (size_t kernelWCoord = 0; kernelWCoord < kernelWidth; kernelWCoord++){
                    sum += kernel[kernelHCoord + kernelWCoord] * data[imgHCoord + startWPositoin + kernelWCoord];
                }
            }
            dst->data[anchorPosition] = (uint8_t)sum;
        }
    }

    return SUCCESS;
}

int erode(ByteImage* image, uint8_t kernelWidth, uint8_t kernelHeight)
{
    if (image == NULL) { return NULL_PTR_ERROR; }
    uint16_t width = image->ROI.width;
    uint16_t height = image->ROI.height;
    uint8_t* data = &image->data[image->ROI.yOffset * image->widthStep + image->ROI.xOffset];

    uint8_t kernelCenterX = kernelWidth / 2;
    uint8_t kernelCenterY = kernelHeight / 2;

    for (size_t i = 0; i < height; i++) {
        uint8_t* imageRowPtr = &data[i * image->widthStep];
        for (size_t j = 0; j < width; j++) {
            if (imageRowPtr[j] == 255) {

                int kernelUplcX = j - kernelCenterX;
                int kernelUplcY = i - kernelCenterY;
                if (kernelUplcX < 0) kernelUplcX = 0;
                if (kernelUplcY < 0) kernelUplcY = 0;

                int kernelEndX = kernelUplcX + kernelWidth;
                int kernelEndY = kernelUplcY + kernelHeight;
                if (kernelEndX > width) { kernelEndX = kernelWidth;}
                if (kernelEndY > height) { kernelEndY = kernelHeight;}

                for (int m = kernelUplcY; m < kernelEndY; m++) {
                    int kernelRow = &data[m * image->widthStep];
                    for (int n = kernelUplcX; n < kernelEndX; n++) {
                        if (kernelRow[n] == 0) {
                            imageRowPtr[j] = 0;
                            break;
                        }
                    }
                }
            }
        }
    }
    return SUCCESS;
}
