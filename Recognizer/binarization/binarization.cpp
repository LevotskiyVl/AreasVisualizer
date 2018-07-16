#include <cmath>
#include <assert.h>

#include "binarization.h"

size_t* calculateIntegralImage(const ByteImage* image)
{
    uint16_t width = image->ROI.width;
    uint16_t height = image->ROI.height;
    size_t* integralImage = (size_t*)calloc(width * height, sizeof(size_t));
    if (integralImage == NULL) {
        return NULL;
    }
    long sum = 0;
    size_t index = 0;
    uint8_t* dataPtr = &image->data[image->ROI.xOffset + (image->ROI.yOffset * image->widthStep)];
    for (int i = 0; i < width; i++) {
        sum = 0;
        for (int j = 0; j < height; j++) {
            index = j * image->widthStep + i;
            sum += dataPtr[index];
            if (i==0)
                integralImage[index] = sum;
            else
                integralImage[index] = integralImage[index-1] + sum;
        }
    }
    return integralImage;
}

size_t getRectFromIntegralImage(size_t* integralImage, int A, int B, int C, int D)
{
    return integralImage[D] - integralImage[C] - integralImage[B] + integralImage[A];
}

double chooseMean(size_t* integralImage, uint16_t width, uint16_t height)
{
    double t = 0;
    int D = (height) * width - 1;
    int C = width - 1;
    int B = ((height - 1) * width)  ;
    int A = 0;

    int all_sum = getRectFromIntegralImage(integralImage, A, B, C, D);

    int average = all_sum/(width * height) ;

    if      (average > 160) t = -0.1f;
    else if (average > 130) t = -0.095f;
    else if (average > 90)  t = -0.09f;
    else if (average > 60)  t = -0.085f;
    else if (average > 30) t = -0.05f;
    else t = 0.05f;
    return t;
}


int bradleyThreshold(const ByteImage* image, int windowSize, double mean, BinarizationMode mode, ByteImage* binarization)
{
    if (image == NULL || binarization == NULL) { return NULL_PTR_ERROR; }

    uint16_t width = image->ROI.width;
    uint16_t height = image->ROI.height;
    if (width == 0 || height == 0) { return SIZES_ERROR; }

    uint8_t* dataPtr = &image->data[image->ROI.xOffset + (image->ROI.yOffset * image->widthStep)];

    size_t* integralImage = calculateIntegralImage(image);
    if (integralImage == NULL) {
        return ALLOCATION_ERROR;
    }

    unsigned char min;
    unsigned char max;
    switch(mode) {
    case NORMAL:
        min = MIN_BINARIZATION_PIXLE_VALUE;
        max = MAX_BINARIZATION_PIXLE_VALUE;
        break;
    case INVERSE:
        min = MAX_BINARIZATION_PIXLE_VALUE;
        max = MIN_BINARIZATION_PIXLE_VALUE;
        break;
    }

    if (mean <= 0.001f && mean >= -0.001f) {
        mean = chooseMean(integralImage, width, height);
    }

    size_t count=0;
    size_t windowCenter = windowSize / 2;
    for (int j = 0; j < height; j++) {
        int dev = j * image->widthStep;
        size_t y1 = j - windowCenter;
        size_t y2 = j + windowCenter;

        if (y1 < 0)
            y1 = 0;
        if (y2 >= height)
            y2 = height-1;
        int tmp2 = y2 - y1;
        int y2_x_width = y2 * image->widthStep;
        int y1_x_width = y1 * image->widthStep;

        for (int i = 0; i < width; i++) {
            size_t index = dev + i;

            int x1 = i - windowCenter;
            int x2 = i + windowCenter;

            if (x1 < 0)
                x1 = 0;
            if (x2 >= width)
                x2 = width-1;

            count = (x2-x1)*tmp2;
            int D = y2_x_width + x2;
            int C = y1_x_width + x2;
            int B = y2_x_width + x1;
            int A = y1_x_width + x1;
            size_t sum = getRectFromIntegralImage(integralImage, A, B, C, D);
            if ((long)(dataPtr[index] * count) < (long)(sum *( 1.0 - mean)))
                binarization->data[index] = min;
            else
                binarization->data[index] = max;
        }
    }
    free(integralImage);
    return 0;
}


double calc_local_stats (uint8_t * input_image, uint16_t image_width, uint16_t image_height, uint8_t * map_m, uint8_t * map_s, int winx, int winy) {

    double m, s, max_s = 0, sum, sum_sq, integral;
    int wxh = (winx) / 2;
    int wyh = (winy) / 2;
    int x_firstth = wxh;
    int y_lastth = image_height - wyh - 1;
    int y_firstth = wyh;
    double winarea = winx * winy;

    max_s = 0;
    for	(int j = y_firstth ; j <= y_lastth; j++) {
        // Calculate the initial window at the beginning of the line
        sum = sum_sq = 0;
        for	(int wy = 0; wy < winy; wy++) {
            for	(int wx = 0; wx < winx; wx++) {
                integral = input_image[image_width * (j - wyh + wy) + wx];
                sum += integral;
                sum_sq += integral * integral;
            }
        }
        m  = sum / winarea;
        s  = sqrt ((sum_sq - (sum * sum) / winarea) / winarea);
        if (s > max_s) {
            max_s = s;
        }
        map_m[j * image_width + x_firstth] = m;
        map_s[j * image_width + x_firstth] = s;

        // Shift the window, add and remove	new/old values to the histogram
        for	(int i = 1; i <= image_width - winx; i++) {
            // Remove the left old column and add the right new column
            for (int wy = 0; wy < winy; wy++) {
                integral = input_image[image_width * (j - wyh + wy) + i - 1];
                sum -= integral;
                sum_sq -= integral * integral;
                integral = input_image[image_width * (j - wyh + wy) + i + winx - 1];
                sum += integral;
                sum_sq += integral * integral;
            }
            m  = sum / winarea;
            s  = sqrt ((sum_sq - (sum * sum) / winarea) / winarea);
            if (s > max_s) {
                max_s = s;
            }
            map_m[j * image_width + i + wxh] = m;
            map_s[j * image_width + i + wxh] = s;
        }
    }

    return max_s;
}

void min_max(uint8_t * arr, size_t length, double * min_val, double * max_val) {
    assert(arr != NULL && length != 0);
    //set initial values
    *min_val = arr[0];
    *max_val = 0;
    //go through array and compare each value
    for (size_t i = 0; i < length; i++) {
        if (arr[i] < *min_val) {
            *min_val = arr[i];
        }
        if (arr[i] > *max_val) {
            *max_val = arr[i];
        }
    }
}

void wolfjolion (uint8_t * input_image, uint16_t image_width, uint16_t image_height,
                 int winx, int winy, double k, BinarizationMode fill_mode, uint8_t* output_data)
{

    double m, s, max_s;
    double th = 0;
    double min_I, max_I;
    int wxh = (winx) / 2;
    int wyh = (winy) / 2;
    int x_firstth = wxh;
    int x_lastth = image_width - wxh - 1;
    int y_lastth = image_height - wyh - 1;
    int y_firstth = wyh;

    // Create local statistics and store them in array
    uint8_t * map_m = (uint8_t * ) calloc(image_width * image_height, sizeof(uint8_t));
    uint8_t * map_s = (uint8_t * ) calloc(image_width * image_height, sizeof(uint8_t));
    max_s = calc_local_stats (input_image, image_width, image_height, map_m, map_s, winx, winy);

    min_max(input_image, image_width * image_height, &min_I, &max_I);

    // Create the threshold surface, including border processing
    uint8_t * thsurf = (uint8_t * ) calloc(image_width * image_height, sizeof(uint8_t));

    for (int j = y_firstth ; j <= y_lastth; j++) {
        // normal, non-border area in the middle of the window
        for	(int i = 0 ; i <= image_width - winx; i++) {
            m = map_m[j * image_width + wxh + i];
            s = map_s[j * image_width + wxh + i];

            th = m + k * (s / max_s - 1) * (m - min_I);
            thsurf[j * image_width + i + wxh] = th;

            if (i == 0) {
                // left border
                for (int c = 0; c <= x_firstth; c++) {
                    thsurf[j * image_width + c] = th;
                }

                // left-upper corner
                if (j == y_firstth) {
                    for (int u = 0; u < y_firstth; u++) {
                        for (int c = 0; c <= x_firstth; c++) {
                            thsurf[u * image_width + c] = th;
                        }
                    }
                }

                // left-lower corner
                if (j == y_lastth) {
                    for (int u = y_lastth + 1; u < image_height; u++) {
                        for (int c = 0; c <= x_firstth; c++) {
                            thsurf[u * image_width + c] = th;
                        }
                    }
                }
            }

            // upper border
            if (j == y_firstth) {
                for (int u = 0; u < y_firstth; ++u) {
                    thsurf[u * image_width + i + wxh] = th;
                }
            }

            // lower border
            if (j == y_lastth) {
                for (int u = y_lastth + 1; u < image_height; ++u) {
                    thsurf[u * image_width + i + wxh] = th;
                }
            }

        }

        // right border
        for (int i = x_lastth; i < image_width; i++) {
            thsurf[j * image_width + i] = th;
        }

        // right-upper corner
        if (j == y_firstth) {
            for (int u =0 ; u < y_firstth; u++) {
                for (int i = x_lastth; i < image_width; i++) {
                    thsurf[u * image_width + i] = th;
                }
            }
        }

        // right-lower corner
        if (j == y_lastth) {
            for (int u = y_lastth + 1; u < image_height; u++) {
                for (int i = x_lastth; i < image_width; i++) {
                    thsurf[u * image_width + i] = th;
                }
            }
        }
    }

    int current_pixel;
    uint8_t low_value, high_value;
    switch (fill_mode) {
    case INVERSE:
        low_value = MAX_BINARIZATION_PIXLE_VALUE;
        high_value = MIN_BINARIZATION_PIXLE_VALUE;
        break;
    case NORMAL:
        low_value = MIN_BINARIZATION_PIXLE_VALUE;
        high_value = MAX_BINARIZATION_PIXLE_VALUE;
        break;
    }

    for (int y = 0; y < image_height; y++) {
        for (int x = 0; x < image_width; x++) {
            current_pixel = y * image_width + x;
            output_data[current_pixel] = input_image[current_pixel] >= thsurf[current_pixel] ? high_value : low_value;
        }
    }
    free(map_m);
    free(map_s);
    free(thsurf);
}

int wolfBinarization(const ByteImage* image, int windowSize, double mean, BinarizationMode mode, ByteImage* binarization)
{
    if (image == NULL) { return NULL_PTR_ERROR; }
    int error = initByteImage(image->ROI.width, image->ROI.height, 1, binarization);
    if (error) { return error; }

    uint8_t* imageData = &image->data[image->ROI.xOffset + image->ROI.yOffset * image->widthStep];
    wolfjolion(imageData, image->ROI.width, image->ROI.height, windowSize, windowSize, mean, mode, binarization->data);
    return 0;
}
