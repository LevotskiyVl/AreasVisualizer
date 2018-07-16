#include <stdio.h>

#include "Recognizer_errors.h"

void printError(int error)
{
    switch (error) {
    case SUCCESS:                  perror("SUCCESS"); break;
    case ZERO_WIDTH_ERROR:         perror("ZERO_WIDTH_ERROR"); break;
    case ZERO_HEIGHT_ERROR:        perror("ZERO_HEIGHT_ERROR"); break;
    case ALLOCATION_ERROR:         perror("ALLOCATION_ERROR"); break;
    case NULL_PTR_ERROR:           perror("NULL_PTR_ERROR"); break;
    case SETTING_ROI_ERROR:        perror("SETTING_ROI_ERROR"); break;
    case WRONG_NUMBER_OF_CHANNELS: perror("WRONG_NUMBER_OF_CHANNELS"); break;
    case FILE_READING_ERROR:       perror("FILE_READING_ERROR"); break;
    case WRONG_PATH:               perror("WRONG_PATH"); break;
    default:                       perror("UNKNOWN ERROR");
    }
}
