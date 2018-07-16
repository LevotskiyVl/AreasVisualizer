#ifndef BINARIZATION_H
#define BINARIZATION_H

#include "Recognizer/byteimage.h"
#include "Recognizer/Recognizer_errors.h"

#define MAX_BINARIZATION_PIXLE_VALUE 255
#define MIN_BINARIZATION_PIXLE_VALUE 0
enum BinarizationMode { NORMAL, INVERSE };
int bradleyThreshold(const ByteImage* image, int windowSize, double mean, BinarizationMode invert, ByteImage* binarization);
int wolfBinarization(const ByteImage* image, int windowSize, double mean, BinarizationMode mode, ByteImage* binarization);

void wolfjolion (uint8_t * input_image, uint16_t image_width, uint16_t image_height,
                 int winx, int winy, double k, BinarizationMode fill_mode, uint8_t* output_data);

#endif //BINARIZATION_H

