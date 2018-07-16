#ifndef BINARIZATION_H
#define BINARIZATION_H

#include "Recognizer/byteimage.h"
#include "Recognizer/Recognizer_errors.h"

#define MAX_BINARIZATION_PIXLE_VALUE 255
#define MIN_BINARIZATION_PIXLE_VALUE 0
enum BinarizationMode { NORMAL, INVERSE };

/**
 * @brief bradleyThreshold Бинаризация бредли
 * @param image указатель на изображение
 * @param windowSize размер окна
 * @param mean сдвиг
 * @param invert тип бинаризация
 * @param binarization указатель на изображение, в которое будет записана бинаризация
 * @return SUCCESS или код ошибки
 */
int bradleyThreshold(const ByteImage* image, int windowSize, double mean, BinarizationMode invert, ByteImage* binarization);

/**
 * @brief wolfBinarization обертка над бинаризацией вульфа
 * @param image указатель на изображение
 * @param windowSize размер окна
 * @param mean сдвиг
 * @param mode тип бинаризация
 * @param binarization указатель на изображение, в которое будет записана бинаризация
 * @return SUCCESS или код ошибки
 */
int wolfBinarization(const ByteImage* image, int windowSize, double mean, BinarizationMode mode, ByteImage* binarization);

/**
 * @brief wolfjolion бинаризация вульфа
 * @param input_image указатель на изображение
 * @param image_width ширина изображения
 * @param image_height высота изображения
 * @param winx ширина окна
 * @param winy высота окна
 * @param k сдвиг
 * @param fill_mode тип бинаризация
 * @param output_data выходное изображение
 */
void wolfjolion (uint8_t * input_image, uint16_t image_width, uint16_t image_height,
                 int winx, int winy, double k, BinarizationMode fill_mode, uint8_t* output_data);

#endif //BINARIZATION_H

