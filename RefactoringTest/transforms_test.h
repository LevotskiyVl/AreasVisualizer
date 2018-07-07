#ifndef TRANSFORMS_TEST_H
#define TRANSFORMS_TEST

#include "image_transofrms/transofrms.h"

TEST(verticalFlipT, successTest){
    ByteImage img;
    uint16_t width = 4;
    uint16_t height = 6;
    uint8_t input[] = { 0,   1,  2,  3,
                        4,   5,  6,  7,
                        8,   9, 10, 11,
                        12, 13, 14, 15,
                        16, 17, 18, 19,
                        20, 21, 22, 23 };

    const uint8_t output[] = { 20, 21, 22, 23,
                               16, 17, 18, 19,
                               12, 13, 14, 15,
                               8,   9, 10, 11,
                               4,   5,  6,  7,
                               0,   1,  2,  3};
    ASSERT_EQ(SUCCESS, createByteImageHeader(width, height, TEST_CHANNELS, &input[0], &img));
    ASSERT_EQ(SUCCESS, verticalFlip(&img));
    for (int i = 0; i < height; i++){
        for (int j = 0;j < width; j++){
            ASSERT_EQ(img.data[i  * width + j], output[i * width + j]);
        }
    }
}

TEST(verticalFlipT, ROITest){
    ByteImage img;
    Rect rect{1, 1, 2, 4};
    uint16_t width = 4;
    uint16_t height = 6;
    uint8_t input[] = { 0,   1,  2,  3,
                        4,   5,  6,  7,
                        8,   9, 10, 11,
                        12, 13, 14, 15,
                        16, 17, 18, 19,
                        20, 21, 22, 23};

    const uint8_t output[] = { 0,   1,  2,  3,
                               4,  17, 18,  7,
                               8,  13, 14, 11,
                               12,  9, 10, 15,
                               16,  5,  6, 19,
                               20, 21, 22, 23};
    ASSERT_EQ(SUCCESS, createByteImageHeader(width, height, TEST_CHANNELS, &input[0], &img));
    ASSERT_EQ(SUCCESS, initROI(&img, rect));
    ASSERT_EQ(SUCCESS, verticalFlip(&img));
    for (int i = 0; i < height; i++){
        for (int j = 0; j < width; j++){
            ASSERT_EQ(img.data[i * width  + j], output[i * width + j]);
        }
    }
}

TEST(verticalFlipT, nullErrors){
    ASSERT_EQ(NULL_PTR_ERROR, verticalFlip(NULL));
}
#endif // TRANSFORMS_TEST
