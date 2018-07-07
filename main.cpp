#include <iostream>

#include "bmp_processor.h"
#include "byteimage.h"
#include "image_transofrms/transofrms.h"
#include <opencv2/highgui.hpp>
#include <opencv2/core.hpp>

using namespace std;
using namespace cv;

void showImage(ByteImage* image, string window)
{
    Mat mat(image->height, image->width, CV_8UC1, image->data);
    imshow(window, mat);
}

int main()
{
    ByteImage image;
    int error = readBmpImage("/home/vlad/Images/lena_gray.bmp", &image);
    if (error) { printError(error); return error; }
    ByteImage blured;
    initByteImage(image.height, image.width, 1, &blured);
    ByteImage copy;
    initByteImage(image.height, image.width, 1, &copy);
    blur(&image, &blured);
    copyPixels(&blured, &copy);
    showImage(&image, "image");
    showImage(&blured, "blur");
    showImage(&copy, "copy");
    cv::waitKey();
    error = saveBmpImage("reult.bmp", &blured);
    if (error) { printError(error); return error; }
    destroyAllWindows();
    return 0;
}
