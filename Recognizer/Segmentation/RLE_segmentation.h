#ifndef RLE_SEGMENTATION
#define RLE_SEGMENTATION

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "Recognizer/byteimage.h"
#include "rleimage.h"

#pragma pack(push,1)
typedef struct SegmentedZone_ {
    Rect rect;
    size_t size;
} SegmentedZone;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SegmentedZonesArray_ {
    SegmentedZone* zones;
    size_t numberOfElements;
} SegmentedZonesArray;
#pragma pack(pop)

enum PixelsConnectionType {CONNECTION_4 = 0, CONNECTION_8 = 1};

SegmentedZonesArray* createSegmentedZonesArray(size_t numberOfZones);
void releaseSegmentedZoneArray(SegmentedZonesArray** segmentedZonesArray);

int RleSegmentation(const ByteImage* image, PixelsConnectionType connectionType,
                    SegmentedZonesArray** zones);

SegmentedZonesArray* rleLabeling(RleImage* rle, PixelsConnectionType connectionType);
void initRleLabeling(const RleImage* rle, size_t** connections, size_t** sz);
bool isCellsConnected(const RleCell* currentCell, const RleCell* upperCell, uint8_t offset);
void processLabelingCollision(size_t label1, size_t label2, size_t* connections, size_t* sz);
void relabeling(RleImage* rle, size_t* connections);
SegmentedZonesArray* collectLabeledZones(const RleImage* rle, size_t lastLabel);
void collectZonesXCoords(const RleImage* rle, size_t lassLabel, SegmentedZonesArray* zones);
void collectZonesYCoords(const RleImage* rle, size_t lassLabel, SegmentedZonesArray* zones);




#endif //RLE_SEGMENTATION
