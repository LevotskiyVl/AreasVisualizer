#include "motion.h"
#include "image_transofrms/transofrms.h"
#include <string.h>

#define GRID_SIZE 16;
#define GRID_ELMTS_FOR_OBJ 30
#define SUBTRACTION_DELTA 15
#define GRIDS_CELL_FILTRATION_THRESH 0.20f
#define SUBTRACTION_DELTA_FX2 20
#define GRIDS_CELL_FILTRATION_THRESH_FX2 0.125f
#define GRIDS_CELL_FILTRATION_THRESH_FX4 0.028f
#define GRIDS_MASK_SIZE 4
#define REDUCE_X 4
#define REDUICE_Y 4
#define BACKGROUND_UPDATING_DELAY 15
#define BACKGROUND_UPDATING_RATE 1
#define MM_MORPH_WIDTH 3
#define MM_MORPH_HEIGHT 3

static uint16_t* calculateHistogram(const ByteImage* image, const MotionDetectorSettings* settings, ByteImage* background);

struct GlobalVariables{
    ByteImage reducedImage;
    ByteImage gaussImage;
    ByteImage backgroundModel;
    ByteImage pixelChanges;
    ByteImage diffImage;
    uint16_t* histogram;
    uint16_t histogramWidth;
    uint16_t histogramHeight;
    MotionDetectorSettings settings;
};

static GlobalVariables globalVariables;

int motionDetector(const ByteImage* image, MotionAreaArray** motinAreasArray)
{
    if (image == NULL) return NULL_PTR_ERROR;
    if (image->width != globalVariables.backgroundModel.width ||
            image->height != globalVariables.backgroundModel.height) {
        return SIZES_ERROR;
    }
    const MotionDetectorSettings* settings = &globalVariables.settings;

    int error = reduceImage(image, settings->reduceStepX, settings->reduceStepY,
                            &globalVariables.reducedImage);
    if (error != SUCCESS) {
        return error;
    }

    error = blur(&globalVariables.reducedImage, &globalVariables.gaussImage);
    if (error != SUCCESS) {
        return error;
    }

    uint16_t* histogram = calculateHistogram(&globalVariables.gaussImage, &globalVariables.settings, &globalVariables.backgroundModel);
    ByteImage map;

}

int initMovementDetector(const ByteImage* sourceImage, MotionDetectorSettings* settings)
{
    static bool wasInitialized = false;
    if (wasInitialized){
        return RE_INITIALIZE;
    }

    //Установка настроек по умолчанию, если настройки не заданы
    if (settings == NULL) {
        fillMotionSettingsWithDefaultValues (&globalVariables.settings);
    }
    else {
        globalVariables.settings = *settings;
    }

    uint16_t reduceWidth = sourceImage->width / globalVariables.settings.reduceStepX;
    uint16_t reduceHeight = sourceImage->height / globalVariables.settings.reduceStepY;

    int error = initByteImage(reduceWidth, reduceHeight,
                              sourceImage->numberOfChannels, &globalVariables.reducedImage);
    if (error) { return error; }

    error = initByteImage(reduceWidth, reduceHeight,
                          sourceImage->numberOfChannels, &globalVariables.gaussImage);
    if (error) { return error; }

    error = initByteImage(reduceWidth, reduceHeight,
                          sourceImage->numberOfChannels, &globalVariables.pixelChanges);
    if (error) { return error; }

    error = initByteImage(reduceWidth, reduceHeight,
                          sourceImage->numberOfChannels, &globalVariables.diffImage);
    if (error) { return error; }

    error = initByteImage(reduceWidth, reduceHeight,
                          sourceImage->numberOfChannels, &globalVariables.backgroundModel);
    if (error) { return error; }
    copyPixels(sourceImage, &globalVariables.backgroundModel);

    globalVariables.histogramWidth = reduceWidth / settings->gridCellSize;
    if (globalVariables.histogramWidth * settings->gridCellSize != reduceWidth) {
        globalVariables.histogramWidth++;
    }

    globalVariables.histogramHeight = reduceHeight / settings->gridCellSize;
    if (globalVariables.histogramHeight * settings->gridCellSize != reduceHeight) {
        globalVariables.histogramHeight++;
    }

    globalVariables.histogram = (uint16_t*)calloc(histogramWidth * histogramHeight, sizeof(uint16_t));
    if (globalVariables.histogram == NULL) { return ALLOCATION_ERROR; }

    wasInitialized = true;
    return 0;
}

void release_movement(){
    releaseByteImage (&globalVariables.backgroundModel);
    releaseByteImage (&globalVariables.gaussImage);
    releaseByteImage (&globalVariables.reducedImage);
    releaseByteImage (&globalVariables.pixelChanges);
    releaseByteImage (&globalVariables.diffImage);

}

int fillMotionSettingsWithDefaultValues(MotionDetectorSettings* motionDetectorSettings)
{
    motionDetectorSettings->gridMaskSize = GRIDS_MASK_SIZE;
    motionDetectorSettings->gridCellSize = GRID_SIZE;
    motionDetectorSettings->gridWithMotionThresh = GRID_ELMTS_FOR_OBJ;
    motionDetectorSettings->minNumberOfElementsInGrid = GRIDS_CELL_FILTRATION_THRESH_FX4;
    motionDetectorSettings->reduceStepX = REDUCE_X;
    motionDetectorSettings->reduceStepY = REDUICE_Y;
    motionDetectorSettings->subtractionThreshold = SUBTRACTION_DELTA;
    motionDetectorSettings->backgroundUpdatingRate = BACKGROUND_UPDATING_RATE;
    motionDetectorSettings->backgroundUpdatingDelay = BACKGROUND_UPDATING_DELAY;
    motionDetectorSettings->mmMorphWidth = MM_MORPH_WIDTH;
    motionDetectorSettings->mmMorphHeight = MM_MORPH_HEIGHT;
}

uint16_t* calculateHistogram(const ByteImage* image, const MotionDetectorSettings* settings, ByteImage* background)
{
    for (uint32_t i = 0; i < globalVariables.histogramHeight; i++) {
        uint32_t cellYCoord = i * settings->gridCellSize;
        uint32_t histRow = i * globalVariables.histogramWidth;

        for (uint32_t j = 0; j < globalVariables->histogramWidth; j++) {
            uint32_t cellXCoord = j * settings->gridCellSize;
            uint32_t numberOfZone = histRow + j;

            globalVariables.histogram[numberOfZone] = calculateHistCell(image, settings, cellXCoord,
                                                                        cellYCoord, background);
        }
    }
}

void createMotionMap(const uint16_t* histogram, const MotionDetectorSettings* settings, ByteImage* motionMap)
{
    const uint16_t mask_square = settings->gridMaskSize * settings->gridMaskSize;
    const uint16_t min_cell_size = mask_square * settings->gridWithMotionThresh;
    for (uint16_t i =0; i < motionMap->height; i++){
        uint16_t num_of_zone = i * motionMap->width;
        for (uint16_t j = 0; j < motionMap->width; j++){
            if (histogram[num_of_zone + j] >= min_cell_size){
                motionMap->data[num_of_zone + j] = 255;
            }
        }
    }
    erode(motionMap, settings->mmMorphWidth, settings->mmMorphHeight);
}

MotionAreaArray* MotionFilter(MotionAreaArray* motionAreas, uint16_t width,uint16_t height, uint8_t mask_size,uint8_t min_size)
{

}


uint16_t calculateHistCell(const ByteImage* image, const MotionDetectorSettings* settings,
                       uint16_t cellXCoord, uint16_t cellYCoord, ByteImage* background)
{
    uint8_t* imgData = image->data;
    uint8_t* backgroundData = background->data;
    uint8_t* changesData = globalVariables.pixelChanges.data;
    uint8_t* diffImageData = globalVariables.diffImage.data;
    memset(diffImageData,0, sizeof(uint8_t) * image->height * image->width);
    uint32_t motionPixelsCntr = 0;

    for (uint16_t m = 0; m < settings->gridCellSize; m++){
        if (cellYCoord + m >= image->height) break;
        uint32_t hPos = (cellYCoord + m) * image->width;
        for (uint16_t n = 0; n < settings->gridCellSize; n++){
            uint32_t wPos = cellXCoord + n;
            if( wPos >= image->width) break;
            //рассчет разности пикселей
            uint32_t pixelPosition = hPos + wPos;
            int diff = imgData[pixelPosition] - backgroundData[pixelPosition];  //diff_map[pixel_pos];//

            //определение направления изменения фона
            uint16_t absDiff = abs(diff);
            int8_t rate = 0;
            if(diff < 0){
                rate = -1;
            }
            if(diff > 0){
                rate = 1;
            }

            if (absDiff < settings->subtractionThreshold){
                changesData[pixelPosition] = 0;
                if ( !((i == 0 && m == 0) || (j == 0 && n == 0) ||
                      diffImageData[pixelPosition - 1] == 0 ||
                      diffImageData[pixelPosition - image->width] == 0 )){

                    diffImageData[pixelPosition] = imgData[pixelPosition];
                    motionPixelsCntr++;

                }
                else {
                    diffImageData[pixelPosition] = 0;
                }
                backgroundData[pixelPosition] += settings->backgroundUpdatingRate * rate;
            }
            else{
                diffImageData[pixelPosition] = imgData[pixelPosition];
                changesData[pixelPosition]++;
                if (changesData[pixelPosition] > settings->backgroundUpdatingDelay){
                    backgroundData[pixelPosition] += settings->backgroundUpdatingRate * rate * 3;
                }
                motionPixelsCntr++;
            }
        }
    }
    return motionPixelsCntr;
}



