#ifndef BMP_PROCESSOR_TEST_H
#define BMP_PROCESSOR_TEST_H

#include "bmp_processor.h"
#include "gtest/gtest.h"

TEST(SAVE_READ_BMP, successTest)
{
    ByteImage readedImage;
    ByteImage savedImage;

    int error = readBmpImage("test.bmp", &readedImage);
    ASSERT_EQ(SUCCESS, error);
    error = saveBmpImage("saved.bmp", &readedImage);
    ASSERT_EQ(SUCCESS, error);
    error = readBmpImage("saved.bmp", &savedImage);
    ASSERT_EQ(SUCCESS, error);
    size_t numberOfBytesInImage = readedImage.height * readedImage.width * readedImage.numberOfChannels;
    for (size_t i = 0; i < numberOfBytesInImage; i++) {
        ASSERT_EQ(readedImage.data[i], savedImage.data[i]);
    }
    releaseByteImage(&readedImage);
    releaseByteImage(&savedImage);
}


#endif // BMP_PROCESSOR_TEST_H
