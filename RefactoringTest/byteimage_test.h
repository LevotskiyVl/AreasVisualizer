#include "byteimage.h"
#include "errors.h"
#include "structs.h"
#include "gtest/gtest.h"

#define TEST_HEIGHT 100
#define TEST_WIDTH 150
#define TEST_CHANNELS 1
static uint8_t data[TEST_HEIGHT * TEST_WIDTH];
TEST(fillByteImageHeaderT, successTest){
    ByteImage img;
    ASSERT_EQ(SUCCESS, fillByteImageHeader(TEST_WIDTH, TEST_HEIGHT, TEST_CHANNELS, &img));
    ASSERT_EQ(TEST_WIDTH, img.width);
    ASSERT_EQ(TEST_HEIGHT, img.height);
    ASSERT_EQ(TEST_CHANNELS, (int)img.numberOfChannels);
    ASSERT_EQ(NULL, img.data);
}

TEST(fillByteImageHeaderT, sizesErrors){
    ByteImage img;
    ASSERT_EQ(ZERO_WIDTH_ERROR, fillByteImageHeader(0, TEST_HEIGHT, TEST_CHANNELS, &img));
    ASSERT_EQ(ZERO_HEIGHT_ERROR, fillByteImageHeader(TEST_WIDTH, 0, TEST_CHANNELS, &img));
    ASSERT_EQ(WRONG_NUMBER_OF_CHANNELS, fillByteImageHeader(1920, TEST_HEIGHT, 0, &img));
}

TEST(createByteImageHeaderT, successTest){
    ByteImage img;
    ASSERT_EQ(SUCCESS, createByteImageHeader(TEST_WIDTH, TEST_HEIGHT, TEST_CHANNELS, data, &img));
    img.data[TEST_WIDTH * TEST_HEIGHT - 1] = 0;
    ASSERT_EQ(0, img.data[TEST_WIDTH * TEST_HEIGHT - 1]);
    ASSERT_EQ(data[TEST_WIDTH * TEST_HEIGHT - 1], img.data[TEST_WIDTH * TEST_HEIGHT - 1]);

}

TEST(createByteImageHeaderT, nullErrors){
    ByteImage img;
    ASSERT_EQ(NULL_PTR_ERROR, createByteImageHeader(TEST_WIDTH, TEST_HEIGHT, TEST_CHANNELS, data, NULL));
    ASSERT_EQ(NULL_PTR_ERROR, createByteImageHeader(TEST_WIDTH, TEST_HEIGHT, TEST_CHANNELS, NULL, &img));
}

TEST(initByteImageT, successTest){
    ByteImage img;
    ASSERT_EQ(SUCCESS, initByteImage(TEST_WIDTH, TEST_HEIGHT, TEST_CHANNELS, &img));
    img.data[TEST_WIDTH * TEST_HEIGHT - 1] = 0;
    ASSERT_EQ(0, img.data[TEST_WIDTH * TEST_HEIGHT - 1]);
    releaseByteImage(&img);
}

TEST(ROIT, successTest){
    ByteImage img;
    Rect rect{10, 10, 20, 20};
    ASSERT_EQ(SUCCESS, initByteImage(TEST_WIDTH, TEST_HEIGHT, TEST_CHANNELS, &img));
    ASSERT_EQ(SUCCESS, initROI(&img, rect));
    //ASSERT_NE(NULL, img.ROI);
    releaseROI(&img);
    ASSERT_EQ(img.height, img.ROI.height);
    ASSERT_EQ(img.width, img.ROI.width);
    ASSERT_EQ(0, img.ROI.xOffset);
    ASSERT_EQ(0, img.ROI.yOffset);
    releaseByteImage(&img);
}

TEST(ROIT, nullErrors){
    ByteImage img;
    Rect rect{0, 0, TEST_WIDTH, TEST_HEIGHT};
    ASSERT_EQ(SUCCESS, initByteImage(TEST_WIDTH, TEST_HEIGHT, TEST_CHANNELS, &img));
    ASSERT_EQ(NULL_PTR_ERROR, initROI(NULL, rect));
    releaseByteImage(&img);
}

TEST(ROIT, SizesError){
    ByteImage img;
    Rect rect{10, 10, 0, 20};
    ASSERT_EQ(SUCCESS, initByteImage(TEST_WIDTH, TEST_HEIGHT, TEST_CHANNELS, &img));
    ASSERT_EQ(SETTING_ROI_ERROR, initROI(&img, rect));
    Rect rect2{10, 10, 20, 10000};
    ASSERT_EQ(SETTING_ROI_ERROR, initROI(&img, rect2));
    releaseROI(&img);
    releaseByteImage(&img);
}

TEST(releaseByteImageT, successTest){
    ByteImage img;
    Rect rect{10, 10, 20, 20};
    ASSERT_EQ(SUCCESS, initByteImage(TEST_WIDTH, TEST_HEIGHT, TEST_CHANNELS, &img));
    ASSERT_EQ(SUCCESS, initROI(&img, rect));
    releaseByteImage(&img);
    ASSERT_EQ(NULL, img.data);
}


