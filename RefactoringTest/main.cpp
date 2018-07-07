#include <iostream>

#include "gtest/gtest.h"

#include "byteimage_test.h"
#include "bmp_processor_test.h"
using namespace std;

int main(int argc, char* argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
