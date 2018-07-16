#include <stdlib.h>
#include <string.h>

#include "byteimage.h"
#include "Recognizer_errors.h"
#include "settings.h"

//Создание и заполнение ByteImage
int fillByteImageHeader(uint16_t width, uint16_t height, uint8_t numOfChannels, ByteImage* image)
{
    if (width == 0) { return ZERO_WIDTH_ERROR; }
    if (height == 0) { return ZERO_HEIGHT_ERROR; }
    if (numOfChannels == 0) { return WRONG_NUMBER_OF_CHANNELS; }
    image->width = width;
    image->height = height;
    image->ROI.xOffset = 0;
    image->ROI.yOffset = 0;
    image->ROI.width = width;
    image->ROI.height = height;
    image->data = NULL;
    image->numberOfChannels = numOfChannels;
    image->widthStep = width * image->numberOfChannels;
    return 0;
}

int createByteImageHeader(uint16_t width, uint16_t height, uint8_t numOfChannels,
                          uint8_t* data, ByteImage* image)
{
    if (image == NULL || data == NULL) { return NULL_PTR_ERROR; }
    int error = fillByteImageHeader(width, height, numOfChannels, image);
    if (error) { return error; }
    image->data = data;
    return 0;
}

int initByteImage(uint16_t width,uint16_t height, uint8_t numOfChannels, ByteImage* image)
{
    int error = fillByteImageHeader(width, height, numOfChannels, image);
    if (error) { return error; }
    image->data = (uint8_t*) calloc(width * height * numOfChannels, sizeof(uint8_t));
    if (image->data == NULL) {
        return ALLOCATION_ERROR;
    }
    return 0;
}

void releaseByteImage(ByteImage* image)
{
    if (image != NULL) {
        if (image->data != NULL) {
            free(image->data);
            image->data = NULL;
        }
    }
}

/////////////////////////////////////////////////////////////////////
//Обработка области интереса
int initROI(ByteImage* parentImage, Rect ROIrectangle)
{
    if (parentImage == NULL || parentImage->data == NULL) {
        return NULL_PTR_ERROR;
    }

    if (parentImage->height < ROIrectangle.y + ROIrectangle.height - 1 ||
        parentImage->width  < ROIrectangle.y + ROIrectangle.width - 1 ||
        ROIrectangle.height * ROIrectangle.width == 0) {
        return SETTING_ROI_ERROR;
    }

    parentImage->ROI.height  = ROIrectangle.height;
    parentImage->ROI.width   = ROIrectangle.width;
    parentImage->ROI.xOffset = ROIrectangle.x;
    parentImage->ROI.yOffset = ROIrectangle.y;
    return 0;
}

void releaseROI(ByteImage *parentImage)
{
    parentImage->ROI.height  = parentImage->height;
    parentImage->ROI.width   = parentImage->width;
    parentImage->ROI.xOffset = 0;
    parentImage->ROI.yOffset = 0;
}

int copyPixels(const ByteImage* src, ByteImage* dst) {
    if (src == NULL || dst == NULL) {
        return NULL_PTR_ERROR;
    }

    if (src->ROI.width != dst->ROI.width ||
            src->ROI.height != dst->ROI.height ||
            src->numberOfChannels != dst->numberOfChannels) {
        return SIZES_ERROR;
    }

    uint16_t width = src->ROI.width;
    uint16_t height = src->ROI.height;
    uint16_t x = src->ROI.xOffset;
    uint16_t y = src->ROI.yOffset;
    uint8_t* srcData = &src->data[y * src->widthStep + x];
    uint8_t* dstData = &dst->data[y * dst->widthStep + x];

    for (uint16_t i = 0; i < height; i++) {
        memcpy(&dstData[i * dst->widthStep], &srcData[i * src->widthStep],
                sizeof(uint8_t) * src->numberOfChannels * width);
    }
    return SUCCESS;
}
