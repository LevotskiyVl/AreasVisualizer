#ifndef ERRORS_H
#define ERRORS_H

// Модуль обработки ошибок. Сожержит коды ошибок и функции для вывода сообщений об ошибке в терминал
//TODO: заменить ошибки 10 и 11 на 19
#define SUCCESS 0
#define ZERO_WIDTH_ERROR 10
#define ZERO_HEIGHT_ERROR 11
#define ALLOCATION_ERROR 12
#define NULL_PTR_ERROR 13
#define SETTING_ROI_ERROR 14
#define WRONG_NUMBER_OF_CHANNELS 15
#define FILE_READING_ERROR 16
#define WRONG_PATH 17
#define RE_INITIALIZE 18
#define SIZES_ERROR 19

void printError(int error);

#endif // ERRORS_H
