#ifndef BMP_PROCESSOR_H
#define BMP_PROCESSOR_H

//Модуль содержит описание функций и структур для работы с BMP изображением.
//Включает в себя функции загрузки и сохранения изображений из и в ByteImage

#include <stdio.h>
#include <stdlib.h>

#include "byteimage.h"
#include "errors.h"

#define BM_CODE 0x4d42
#define COLOR_PIXELS_SIZE 1024
#define XPelsPerMeter 0xb13
#define YPelsPerMeter 0xb13

#pragma pack(push, 1)
typedef struct FILEHEADER_
{
    short bfType;                 //Отличие формата (BM)
    int   bfSize;                 //размер файла в байтах
    short Reserved1;              //резерв 0
    short Reserved2;              //резерв 0
    int   bfOffBits;              //Положение пиксельных данных относительно начала данной структуры (в байтах).
} FILEHEADER;
#pragma pack(pop)

#pragma pack(push,1)
typedef struct INFOHEADER_
{
    int   biSize;                 //Размер данной структуры в байтах
    int   biWidth;                //Ширина растра в пикселях. Указывается целым числом со знаком.
    int   biHeight;               //высота растра в пикселях (абсолютное значение числа) и порядок следования строк в двумерных массивах (знак числа)
    short biPlanes;               //В BMP допустимо только значение 1.
    short biBitCount;             //Количество бит на пиксель
    int   biCompression;          //Указывает на способ хранения пикселей
    int   biSizeImage;            //Размер пиксельных данных в байтах.
    int   biXPelsPerMeter;        //Количество пикселей на метр по горизонтали и вертикали
    int   biYPelsPerMeter;
    int   biClrUsed;              //Размер таблицы цветов в ячейках
    int   biClrImportant;         //Количество ячеек от начала таблицы цветов до последней используемой (включая её саму).
} INFOHEADER;
#pragma pack(pop)

/**
 * @brief readBmpFileHeader чтение струткруы FILEHEADER из файла
 * @param dst структура в которую будет записана прочитаная информация
 * @param stream указатель на файл
 * @return возвращает 0 в случае успеха или код ошибки
 */
int readBmpFileHeader(FILE *stream, FILEHEADER* dst);

/**
 * @brief fillBmpFileHeader заполняет структуру filheader исходя из харакатеристик изображения
 * @param img указатель на изображение
 * @param header указатель на заполняемую структуру
 */
void fillBmpFileHeader(const ByteImage *img, FILEHEADER* header);

/**
 * @brief readBmpInfoHeader чтение струткруы InfoHeader из файла
 * @param dst структура в которую будет записана прочитаная информация
 * @param stream указатель на файл
 * @return возвращает 0 в случае успеха или код ошибки
 */
int readBmpInfoHeader(FILE* stream, INFOHEADER* dst);

/**
 * @brief fillBmpInfoHeader заполняет структуру infoheader исходя из харакатеристик изображения
 * @param img указатель на изображение
 * @param header указатель на заполняемую структуру
 */
void fillBmpInfoHeader(const ByteImage* img, INFOHEADER* header);

/**
 * @brief readBmpPixelArray Чтение пикселей изображения
 * @param width ширина изображения
 * @param height высота изображения
 * @param bytesOnPixel количество байт на пиксель
 * @param padding смещение в конце строки
 * @param stream указатель на файл
 * @param dst указатель на массив, куда запишется результат чтения
 */
void readBmpPixelArray(size_t width, size_t height,
                       uint8_t bytesOnPixel, size_t padding,
                       FILE *stream, uint8_t* dst);

/**
 * @brief saveBmpHeader Создание и запись Bmp заголовка в файл
 * @param image сохраняемого изображение
 * @param stream указатель на файл, в который выполняется сохранение
 */
void saveBmpHeader(const ByteImage *image, FILE *stream);

/**
 * @brief readBmpPixelArray Чтение пикселей изображения
 * @param src указатель на массив, куда запишется результат чтения
 * @param width ширина изображения
 * @param height высота изображения
 * @param bytesOnPixel количество байт на пиксель
 * @param padding смещение в конце строки
 * @param stream указатель на файл
 */
void saveBmpPixelArray(const ByteImage *image, FILE* stream);

/**
 * @brief readBmpImage чтение Bmp изображения
 * @param path путь к файлу
 * @param img указатель на изображение в которое будет записана прочитанная изображения
 * @return возвращает 0 в случае успеха или код ошибки
 */
int readBmpImage(const char* path, ByteImage* img);

/**
 * @brief saveBmpImage сохранение Bmp изображения
 * @param path путь для сохраняемого файла
 * @param img сохраняемое изображение
 * @return возвращает 0 в случае успеха, иначе код ошибки
 */
int saveBmpImage(const char* path, const ByteImage *img);

#endif // BMP_PROCESSOR_H
