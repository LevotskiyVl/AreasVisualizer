#include <string.h>

#include "motion.h"
#include "Recognizer/settings.h"
#include "Recognizer/image_transofrms/transofrms.h"

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

int motionDetector(const ByteImage* image, MotionAreaArray** motionAreasArray)
{
    *motionAreasArray = NULL;
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

    uint16_t mapHeight = globalVariables.reducedImage.height / settings->gridMaskSize;
    uint16_t mapWidth = globalVariables.reducedImage.width / settings->gridMaskSize;
    if (mapWidth * settings->gridMaskSize != globalVariables.reducedImage.width) mapWidth++;
    if (mapHeight * settings->gridMaskSize != globalVariables.reducedImage.height) mapHeight++;
    ByteImage map;
    initByteImage(mapWidth, mapHeight, 1, &map);

    createMotionMap(histogram, settings, &map);

    SegmentedZonesArray* segmentedZones = NULL;
    error = RleSegmentation(&map, CONNECTION_8, &segmentedZones);

    releaseByteImage(&map);
    if (error) {
        return error;
    }
    if (segmentedZones == NULL) {
        return 0;
    }

    MotionAreaArray* motionFrames = segmentedZonesToMotionAreas(segmentedZones);
    releaseSegmentedZoneArray(&segmentedZones);
    if (motionFrames == NULL) {
        return 0;
    }

    *motionAreasArray = motionFilter(motionFrames, settings);
    scalingMotionAreas(*motionAreasArray, settings);

    return 0;
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

    globalVariables.histogram = (uint16_t*)calloc(globalVariables.histogramWidth *
                                                  globalVariables.histogramHeight, sizeof(uint16_t));
    if (globalVariables.histogram == NULL) { return ALLOCATION_ERROR; }

    wasInitialized = true;
    return 0;
}

void releaseMovementDetector()
{
    releaseByteImage (&globalVariables.backgroundModel);
    releaseByteImage (&globalVariables.gaussImage);
    releaseByteImage (&globalVariables.reducedImage);
    releaseByteImage (&globalVariables.pixelChanges);
    releaseByteImage (&globalVariables.diffImage);

}

void fillMotionSettingsWithDefaultValues(MotionDetectorSettings* motionDetectorSettings)
{
    motionDetectorSettings->gridMaskSize = GRIDS_MASK_SIZE;
    motionDetectorSettings->gridCellSize = GRID_SIZE;
    motionDetectorSettings->cellWithMotionThresh = GRIDS_CELL_FILTRATION_THRESH_FX4;
    motionDetectorSettings->minNumberOfMotionPixelsForZone = GRID_ELMTS_FOR_OBJ;
    motionDetectorSettings->reduceStepX = REDUCE_X;
    motionDetectorSettings->reduceStepY = REDUICE_Y;
    motionDetectorSettings->subtractionThreshold = SUBTRACTION_DELTA;
    motionDetectorSettings->backgroundUpdatingRate = BACKGROUND_UPDATING_RATE;
    motionDetectorSettings->backgroundUpdatingDelay = BACKGROUND_UPDATING_DELAY;
    motionDetectorSettings->mmMorphWidth = MM_MORPH_WIDTH;
    motionDetectorSettings->mmMorphHeight = MM_MORPH_HEIGHT;
    motionDetectorSettings->nesstingsOffsetForMotionZone = NESSTINGS_OFFSET_FOR_MOTION;
}

uint16_t* calculateHistogram(const ByteImage* image, const MotionDetectorSettings* settings, ByteImage* background)
{
    for (uint32_t i = 0; i < globalVariables.histogramHeight; i++) {
        uint32_t cellYCoord = i * settings->gridCellSize;
        uint32_t histRow = i * globalVariables.histogramWidth;

        for (uint32_t j = 0; j < globalVariables.histogramWidth; j++) {
            uint32_t cellXCoord = j * settings->gridCellSize;
            uint32_t numberOfZone = histRow + j;

            globalVariables.histogram[numberOfZone] = calculateHistCell(image, settings, cellXCoord,
                                                                        cellYCoord, background);
        }
    }
    return globalVariables.histogram;
}

void createMotionMap(const uint16_t* histogram, const MotionDetectorSettings* settings, ByteImage* motionMap)
{
    const uint16_t mask_square = settings->gridMaskSize * settings->gridMaskSize;
    const uint16_t motionPixelsThresh = mask_square * settings->cellWithMotionThresh;
    for (uint16_t i =0; i < motionMap->height; i++){
        uint16_t num_of_zone = i * motionMap->width;
        for (uint16_t j = 0; j < motionMap->width; j++){
            if (histogram[num_of_zone + j] >= motionPixelsThresh){
                motionMap->data[num_of_zone + j] = 255;
            }
        }
    }
    erode(motionMap, settings->mmMorphWidth, settings->mmMorphHeight);
}

MotionAreaArray* motionFilter(MotionAreaArray* motionAreasArray, const MotionDetectorSettings* settings)
{
    uint8_t* areasForDeleting = (uint8_t*) calloc(motionAreasArray->numberOfElements, sizeof(uint8_t));
    uint16_t numberOfDeletedAreas = 0;
    for (size_t i = 0; i < motionAreasArray->numberOfElements; i++) {
        bool isValid = checkMotionArea(&motionAreasArray->motionAreas[i],
                                       settings);
        if (isValid) {
            isValid = checkNesting(&motionAreasArray->motionAreas[i],
                                   motionAreasArray, areasForDeleting, settings);
        }
        if (!isValid) {
            areasForDeleting[numberOfDeletedAreas] = 1;
            numberOfDeletedAreas++;
        }
    }

    uint16_t resultArraySize = motionAreasArray->numberOfElements - numberOfDeletedAreas;
    MotionAreaArray* resultArray = createMotionAreaArray(resultArraySize);
    if (resultArray == NULL) {
        free(areasForDeleting);
        return NULL;
    }

    fillFiltredMotionAreaArray(motionAreasArray, areasForDeleting, resultArray);
    free(areasForDeleting);
    return resultArray;
}

void fillFiltredMotionAreaArray(const MotionAreaArray* motionAreasArray,
                                const uint8_t* areasForDeleting, MotionAreaArray* resultArray)
{
    uint16_t numberOfCopiedFrames = 0;
    for (int i = 0; i < motionAreasArray->numberOfElements; i++) {
        if (areasForDeleting[i] == 0) {
            memcpy(&resultArray->motionAreas[numberOfCopiedFrames], &motionAreasArray->motionAreas[i], sizeof(MotionArea));
        }
    }
}


bool checkMotionArea(MotionArea* motionArea, const MotionDetectorSettings* settings)
{
    if (motionArea->numberOfMotionPixels >= settings->minNumberOfMotionPixelsForZone){
        return true;
    }
    return false;
}

bool checkNesting(MotionArea* motionArea, MotionAreaArray* areas,
                  const uint8_t* areasForDeleting, const MotionDetectorSettings* settings)
{
    bool returnValue = false;
    Rect* areaRect = &motionArea->MovementRect;
    for (size_t  i = 0; i < areas->numberOfElements; i++) {
        if (motionArea == &areas->motionAreas[i] || areasForDeleting[i] == 1 ||
            areas->motionAreas[i].numberOfMotionPixels < settings->minNumberOfMotionPixelsForZone) {
            continue;
        }
        Rect* checkingRect = &areas->motionAreas[i].MovementRect;
        if (isRectInside(areaRect, checkingRect, settings->nesstingsOffsetForMotionZone)){
            returnValue = true;
            *checkingRect = mergeRects(areaRect, checkingRect);
        }
    }
    return returnValue;
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
                if ( !((cellYCoord == 0 && m == 0) || (cellXCoord == 0 && n == 0) ||
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

void scalingMotionAreas(MotionAreaArray* areas, const MotionDetectorSettings* settings)
{
    uint16_t scaleXCoeff = settings->reduceStepX * settings->gridMaskSize;
    uint16_t scaleYCoeff = settings->reduceStepX * settings->gridMaskSize;
    for (int i = 0; i < areas->numberOfElements; i++) {
        Rect* rect = &areas->motionAreas[i].MovementRect;
        rect->x *= scaleXCoeff;
        rect->y *= scaleYCoeff;
        rect->width *= scaleXCoeff;
        rect->height *= scaleYCoeff;
    }
}

MotionAreaArray* createMotionAreaArray(uint16_t numberOfElements) {
    if (numberOfElements == 0) {
        return NULL;
    }

    MotionAreaArray* out = (MotionAreaArray*) malloc(sizeof(MotionAreaArray));
    if (out == NULL) {
        return NULL;
    }

    out->motionAreas = (MotionArea*)calloc(numberOfElements, sizeof(MotionArea));
    if (out->motionAreas == NULL) {
        free(out);
        return NULL;
    }

    out->numberOfElements = numberOfElements;
    return out;
}

void releaseMotionAreaArray(MotionAreaArray** motionAreaArray) {
    if (*motionAreaArray == NULL) {
        return;
    }

    if ((*motionAreaArray)->motionAreas != NULL) {
        free((*motionAreaArray)->motionAreas);
        (*motionAreaArray)->motionAreas = NULL;
    }
    free(*motionAreaArray);
    *motionAreaArray = NULL;
}

MotionAreaArray* segmentedZonesToMotionAreas(const SegmentedZonesArray* segmentedZones)
{
    MotionAreaArray* motionAreas = createMotionAreaArray(segmentedZones->numberOfElements);
    if (motionAreas == NULL) {
        return NULL;
    }

    for (size_t i = 0; i < segmentedZones->numberOfElements; i++){
        motionAreas->motionAreas[i].MovementRect = segmentedZones->zones[i].rect;
        motionAreas->motionAreas[i].numberOfMotionPixels = segmentedZones->zones[i].size;
    }

    return motionAreas;
}
