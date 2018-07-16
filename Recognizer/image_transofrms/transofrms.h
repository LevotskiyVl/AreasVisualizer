#ifndef TRANSOFRMS_H
#define TRANSOFRMS_H

//TODO: перенести сюда переворот изображения, функцию для врисовывания одного изображения в другое,
//TODO: прореживание, интерполяцию свёртку
//TODO: Распараллеливанье обработки
#include "Recognizer/byteimage.h"

/**
 * @brief reduceImage - прореживанье изоображения. Операция уменьшает изображение в указанное
 * количество раз за счёт удаления строк и столбцов не соответсвующих шагу
 * @param src - исходное изображения
 * @param reduceStepX - шаг прореживания по X
 * @param reduceStepY - шаг прореживания по Y
 */
int reduceImage(const ByteImage* src, uint8_t reduceStepX, uint8_t reduceStepY, ByteImage* dst);

/**
 * @brief verticalFlip Зеркальное отражение изображения по вертикали
 * @param img Изображение для отражения
 * @return Код ошибки или 0
 */
int verticalFlip(ByteImage* img);

/**
 * @brief blur размытие изображения с понижением яркости в 0.75 раз
 * @param src исходное изображение
 * @param dst изображение после размытия
 * @return Код ошибки или 0
 */
int blur(const ByteImage* src, ByteImage* dst);

/**
 * @brief convolution свёртка изображения с заданным ядром
 * @param src исходное изображение
 * @param kernel параметры ядра свёртки
 * @param kernelWidth ширина ядра свёртки
 * @param kernelHeight высота ядра свёртки
 * @param dst результат
 * @return Код ошибки или 0
 */
int convolution(const ByteImage* src, const double* kernel, size_t kernelWidth,
                size_t kernelHeight, ByteImage* dst);

/**
 * @brief erode эрозия бинаризованного изображения
 * @param image указатель на изображения
 * @param kernelWidth ширина изображения
 * @param kernelHeight высота изображения
 * @return код ошибки или 0
 */
int erode(ByteImage* image, uint8_t kernelWidth, uint8_t kernelHeight);


#endif // TRANSOFRMS_H
