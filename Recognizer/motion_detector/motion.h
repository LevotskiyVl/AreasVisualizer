#ifndef MOTION_H
#define MOTION_H

//Модуль детектора движения
//TODO: Реализация функций, настройки фильтра, документирование, тесты
#include <stdint.h>
#include <stdbool.h>

#include "Recognizer/byteimage.h"
#include "Recognizer/structs.h"
#include "Recognizer/Recognizer_errors.h"
#include "Recognizer/Segmentation/RLE_segmentation.h"

typedef struct MotionDetectorSettings_{
    uint8_t gridCellSize;
    uint8_t minNumberOfMotionPixelsForZone;
    uint8_t subtractionThreshold;
    uint8_t gridMaskSize;
    uint8_t reduceStepY;
    uint8_t reduceStepX;
    uint8_t backgroundUpdatingDelay;
    uint8_t backgroundUpdatingRate;
    uint8_t mmMorphWidth;
    uint8_t mmMorphHeight;
    uint8_t nesstingsOffsetForMotionZone;
    float cellWithMotionThresh;
} MotionDetectorSettings;

typedef struct MotionArea_{
    Rect MovementRect;
    uint16_t numberOfMotionPixels;
} MotionArea;

typedef struct MotionAreaArray_{
    MotionArea* motionAreas;
    uint16_t numberOfElements;
} MotionAreaArray;

int initMovementDetector(const ByteImage* sourceImage, MotionDetectorSettings* settings);
void releaseMovementDetector();

MotionAreaArray* createMotionAreaArray(uint16_t numberOfElements);
void releaseMotionAreaArray(MotionAreaArray**);
int motionDetector(const ByteImage* image, MotionAreaArray** motionAreasArray);

void fillMotionSettingsWithDefaultValues(MotionDetectorSettings* motionArea);

uint16_t* calculateHistogram(const ByteImage* image, const MotionDetectorSettings *settings, ByteImage* background);
uint16_t calculateHistCell(const ByteImage* image, const MotionDetectorSettings* settings,
                                  uint16_t cellXCoord, uint16_t cellYCoord, ByteImage* background);

void createMotionMap(const uint16_t* histogram, const MotionDetectorSettings* settings, ByteImage* motionMap);

MotionAreaArray* motionFilter(MotionAreaArray* motionAreasArray, const MotionDetectorSettings* settings);
bool checkMotionArea(MotionArea* motionArea, const MotionDetectorSettings* settings);
bool checkNesting(MotionArea* motionArea, MotionAreaArray* areas,
                  const uint8_t* areasForDeleting, const MotionDetectorSettings *settings);
void fillFiltredMotionAreaArray(const MotionAreaArray* motionAreasArray,
                                const uint8_t* areasForDeleting, MotionAreaArray* resultArray);

void scalingMotionAreas(MotionAreaArray* areas, const MotionDetectorSettings* settings);
MotionAreaArray* segmentedZonesToMotionAreas(const SegmentedZonesArray* segmentedZones);


#endif //MOTION_H
