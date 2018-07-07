#ifndef MOTION_H
#define MOTION_H

//Модуль детектора движения
//TODO: Реализация функций, настройки фильтра, документирование, тесты
#include <stdint.h>
#include <stdbool.h>

#include "byteimage.h"
#include "structs.h"
#include "errors.h"


typedef struct MotionDetectorSettings_{
    uint8_t gridCellSize;
    uint8_t minNumberOfElementsInGrid;
    uint8_t subtractionThreshold;
    uint8_t gridMaskSize;
    uint8_t reduceStepY;
    uint8_t reduceStepX;
    uint8_t backgroundUpdatingDelay;
    uint8_t backgroundUpdatingRate;
    uint8_t mmMorphWidth;
    uint8_t mmMorphHeight;
    float gridWithMotionThresh;
} MotionDetectorSettings;

typedef struct MotionArea_{
    Rect MovementRect;
} MotionArea;

typedef struct MotionAreaArray_{
    MotionArea* motionAreas;
    uint16_t numberOfElements;
} MotionAreaArray;

int motionDetector(const ByteImage* image, MotionAreaArray*);
static uint16_t* calculateHistogram(const ByteImage* image, const MotionDetectorSettings *settings, ByteImage* background);
static int createMotionMap(uint16_t* histogram, ByteImage* motionMap, uint16_t mask_size, float roi_thresh);
static int fillMotionSettingsWithDefaultValues(MotionDetectorSettings* motionArea);
static MotionAreaArray* MotionFilter(MotionAreaArray* motionAreas, uint16_t width,uint16_t height, uint8_t mask_size,uint8_t min_size);


#endif //MOTION_H
