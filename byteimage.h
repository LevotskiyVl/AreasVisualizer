#ifndef BYTEIMAGE_H
#define BYTEIMAGE_H

/*Файл содержит описание стуктуры ByteImage, которая хранит изображение байтах,
 * а также функции для её инициализации, обработки и удаления*/

#include <stdint.h>
#include <stdlib.h>

#include "structs.h"

/**
* @brief Область интереса изображения.
* @param width ширина изображения
* @param height высота изображения
* @param xOffset начало области интереса по х
* @param yOffset начало области интереса по y
*/
typedef struct ByteImageROI_{
    uint16_t xOffset, yOffset;
    uint16_t width, height;
}ByteImageROI;

/**
* @brief Контейнер изображения.
* @param data указатель на массив пикселей
* @param ROI указатель на информацию об области интереса.
* Если не равен NULL будет обрабатываться только область интереса.
* @param numberOfChannels кол-во байт до следющего пикселя
* @param widthStep кол-во байт между стркоами изображениями
* @param width ширина изображения
* @param height высота изображения
*/
typedef struct ByteImage_{
    uint8_t* data;
    ByteImageROI ROI;
    uint16_t widthStep;
    uint16_t width, height;
    size_t numberOfChannels;
}ByteImage;

/**
 * @brief fillByteImageHeader заполнение заголовка byteImageHeader. Без выделения памяти
 * @param width ширина изобаржения
 * @param height высота изображения
 * @param numOfChannels количество каналов
 * @param image заполняемая структура
 * @return 0 в случае успеха, иначе код ошибки
 */
int fillByteImageHeader(uint16_t width, uint16_t height, uint8_t numOfChannels, ByteImage* image);
/**
 * @brief createByteImageHeader создаёт ByteImage без выделения памяти под изображение.
 * Вместо этого в качестве изображения используется data
 * @param width ширина изображения
 * @param height высота изображения
 * @param image указатель на инициализируемое изображение
 * @param data указатель на выделенную память
 * @return возвращает 0 в слуаче успеха или код ошибки
 */
int createByteImageHeader(uint16_t width, uint16_t height, uint8_t numOfchannels,
                          uint8_t* data, ByteImage* image);

/**
 * @brief initByteImage Инициализация и выделение памяти под изображения.
 * Выделеная память должна быть освобождена вызововом releaseByteImage
 * @param width ширина изображения
 * @param height высота изображения
 * @param image указатель на инициализируемое изображение
 * @return возвращает 0 в слуаче успеха или код ошибки
 */
int initByteImage(uint16_t width, uint16_t height, uint8_t numOfchannels, ByteImage *image);

/**
 * @brief releaseByteImage Освобождение памяти из под изображения
 * @param image Указатель на изображения
 */
void releaseByteImage(ByteImage* image);

/**
 * @brief initROI Инициализация области интереса в изображении. Выделения памяти не происходит.
 * @param parentImage Указатель на изобраюжение, в котором выделяется область инетереса.
 * @param ROIrectangle Координаты и размеры области интереса
 * @return Возвращает 0 в случае успеха или код ошибки
 */
int initROI(ByteImage* parentImage, Rect ROIrectangle);

/**
 * @brief releaseROI Освобождает память, выделенную под изображение
 * @param parentImage указатель на изображение
 */
void releaseROI(ByteImage* parentImage);

/**
 * @brief copyByteImage копирование изображениет
 * @param src исходное изображение
 * @param dst результат
 * @return код ошибки или 0
 */
int copyPixels(const ByteImage* src, ByteImage* dst);



#endif // BYTEIMAGE_H
