#include <string.h>

#include "RLE_segmentation.h"
#include "Recognizer/settings.h"

SegmentedZonesArray* createSegmentedZonesArray(size_t numberOfElements)
{
    if (numberOfElements == 0) {
        return NULL;
    }

    SegmentedZonesArray* out = (SegmentedZonesArray*) malloc(sizeof(SegmentedZonesArray));
    if (out == NULL) {
        return NULL;
    }

    out->zones = (SegmentedZone*) calloc(numberOfElements, sizeof(SegmentedZone));
    if (out->zones == NULL) {
        free(out);
        return NULL;
    }

    out->numberOfElements = numberOfElements;
    return out;
}

void releaseSegmentedZoneArray(SegmentedZonesArray** segmentedZonesArray)
{
    if (*segmentedZonesArray == NULL) {
        return;
    }

    if ((*segmentedZonesArray)->zones != NULL) {
        free((*segmentedZonesArray)->zones);
        (*segmentedZonesArray)->zones = NULL;
    }
    free(*segmentedZonesArray);
    *segmentedZonesArray = NULL;
}


int RleSegmentation(const ByteImage* image, PixelsConnectionType connectionType,
                    SegmentedZonesArray** zones)
{
    RleImage rle;
    int error = createRleImage(image, &rle);
    *zones = NULL;
    if (error) { return error; }
    *zones = rleLabeling(&rle, connectionType);
    releaseRleImage(&rle);
}

SegmentedZonesArray* rleLabeling(RleImage* rle, PixelsConnectionType connectionType)
{
    //Инициализация массивов для поиска связных компонент изображения
    size_t* connections = NULL;
    size_t* sz = NULL;
    initRleLabeling(rle, &connections, &sz);
    if (sz == NULL || connections == NULL) {
        return NULL;
    }

    uint8_t offset;
    switch (connectionType) {
    case CONNECTION_4: offset = 0; break;
    case CONNECTION_8: offset = 1; break;
    }

    size_t lastLabel = 0;
    RleRow* upperRow = NULL;
    RleRow* processingRow = &rle->rows[0];
    //Маркировка первой строки
    for (size_t i = 0; i < processingRow->numberOfCells; i++) {
        processingRow->cells[i].label = ++lastLabel;
    }

    //Маркировка всего изображения
    for (size_t i = 1; i < rle->height; i++) {
        upperRow = &rle->rows[i - 1];
        processingRow = &rle->rows[i];

        for (size_t j = 0; j < processingRow->numberOfCells; j++) {
            int currentCellEnd = processingRow->cells[j].end;
            //проверяем не соприкасается ли маркируемый объект с объектами верхней строки
            for (size_t k = 0; k < upperRow->numberOfCells; k++) {
                //получаем позицию начала и конца объекта верхней строки
                //для реализации 8-связного алгоритма расширяем границы объекта
                int upperCellStart = upperRow->cells[k].begin - offset;
                //как только позиция верхнего объекта окажется за послденей точкой маркируемого - выходим из цикла
                if (upperCellStart > currentCellEnd) break;
                //проверяем различные ситуации связанности объектов
                if (isCellsConnected(&processingRow->cells[j], &upperRow->cells[k], offset)) {
                    //если объекты связаны
                    //и у маркируемого метки нет, то присваиваем метку связанного объекта
                    if (processingRow->cells[j].label == 0) {
                        processingRow->cells[j].label = upperRow->cells[k].label;
                    }
                    else {
                        //если метка есть, то запоминаем связанные метки
                        processLabelingCollision(processingRow->cells[j].label, upperRow->cells[k].label,
                                                 connections, sz);
                    }
                }
            }
            //если связей нет, то присваиваем новую метку
            if (processingRow->cells[j].label == 0) {
                processingRow->cells[j].label = ++lastLabel;
            }
        }
    }
    if (lastLabel == 0){ return NULL; }
    //выполняем перемаркировку
    relabeling(rle, connections);
    SegmentedZonesArray* zones = collectLabeledZones(rle, lastLabel);
    return zones;
}

void initRleLabeling(const RleImage* rle, size_t** connections, size_t** sz)
{
    //TODO: Если для больших участкво изображения инициализация
    //будет занимать слишком много времени - реализовать менеджер памяти,
    //который должен выдавать указатели на участки большого массива

    size_t maxNumberOfObjects = rle->height * rle->width / 2;
    if (maxNumberOfObjects > MAX_FULL_HD_OBJTS) {
        maxNumberOfObjects = MAX_FULL_HD_OBJTS;
    }

    *sz = (size_t*) malloc(maxNumberOfObjects * sizeof(size_t));
    if (*sz == NULL) {
        connections = NULL;
        return;
    }
    memset(*sz, 1, maxNumberOfObjects * sizeof(size_t));

    *connections = (size_t*) malloc(maxNumberOfObjects * sizeof(size_t));
    if (*connections == NULL) {
        free(*sz);
        *sz = NULL;
        return;
    }

    for (size_t i = 0 ; i < maxNumberOfObjects; i++) {
        (*connections[i]) = i;
    }
}


bool isCellsConnected(const RleCell* currentCell, const RleCell* upperCell, uint8_t offset)
{
    int start_pos = currentCell->begin;
    int end_pos = currentCell->end;

    int upper_start = upperCell->begin - offset;
    int upper_end = upperCell->end + offset;

    if ( (start_pos >= upper_start && end_pos <= upper_end)    ||
         (start_pos >= upper_start && start_pos <= upper_end)  ||
         (end_pos >= upper_start && end_pos <= upper_end)      ||
         (upper_start >= start_pos && upper_end <= end_pos)){
        return true;
    }
    return false;
}

void processLabelingCollision(size_t label1, size_t label2, size_t* connections, size_t* sz)
{
    size_t l, m;
    for (l = label1; l != connections[l]; l=connections[l]);
    for (m = label2; m != connections[m]; m=connections[m]);
    if (l != m) {
        if (sz[l] < sz[m]) {
            connections[l] = m; sz[m] += sz[l];
        }
        else { connections[m] = l; sz[l] += sz[m]; }
    }
}

void relabeling(RleImage* rle, size_t* connections)
{
    for (size_t i=0; i < rle->height; i++){
        RleRow* processingRow = &rle->rows[i];
        for (size_t j = 0; j < processingRow->numberOfCells; j++){
            size_t k=0;
            size_t const val_what = processingRow->cells[j].label;
            for (k = val_what; k != connections[k]; k =connections[k]);
            size_t const val_by   = connections[k];
            size_t const mask     = (0 - (val_what && val_by));
            processingRow->cells[j].label = ((mask & val_by) | ((~mask) & val_what));
            if (val_what) {
                if (val_by) {
                    processingRow->cells[j].label = val_by;
                }
            }
        }
    }
}

SegmentedZonesArray* collectLabeledZones(const RleImage* rle, size_t lastLabel)
{
    SegmentedZonesArray* zones = createSegmentedZonesArray(lastLabel);
    if (zones == NULL) { return NULL; }

    collectZonesYCoords(rle, lastLabel, zones);
    collectZonesXCoords(rle, lastLabel, zones);

    return zones;
}

void collectZonesYCoords(const RleImage* rle, size_t lastLabel, SegmentedZonesArray* zones)
{
    uint8_t* firstRowInitialized = (uint8_t*) calloc(lastLabel, sizeof(uint8_t));
    for (size_t i = 0; i < rle->height; i++) {
        const RleRow* processingRow = &rle->rows[i];
        for (size_t j = 0; j < processingRow->numberOfCells; j++) {
            size_t label = processingRow->cells[j].label;
            if (label) {
                SegmentedZone* zone = &zones->zones[label - 1];
                zone->size += processingRow->cells[j].end - processingRow->cells[j].begin + 1;

                if (!firstRowInitialized[label]) {
                    zone->rect.y = i;
                    firstRowInitialized[label] = 1;
                }

                zone->rect.height = i - zone->rect.y + 1;
            }
        }
    }
    free(firstRowInitialized);
}

void collectZonesXCoords(const RleImage* rle, size_t lastLabel, SegmentedZonesArray* zones)
{
    uint8_t* firstColInitialized = (uint8_t*) calloc(lastLabel, sizeof(uint8_t));
    for (size_t i = 0; i < rle->height; i++) {
        const RleRow* processingRow = &rle->rows[i];
        for (size_t j = 0; j < processingRow->numberOfCells; j++) {
            size_t label = processingRow->cells[j].label;
            if (label) {
                SegmentedZone* zone = &zones->zones[label - 1];

                if (!firstColInitialized[label] ||
                    zone->rect.x > processingRow->cells[j].begin) {
                    zone->rect.x = processingRow->cells[j].begin;
                    firstColInitialized[label] = 1;
                }

                int32_t newWidth = processingRow->cells[j].end -zone->rect.x + 1;
                if (zone->rect.width < newWidth) {
                    zone->rect.width = newWidth;
                }
            }
        }
    }
    free(firstColInitialized);
}
