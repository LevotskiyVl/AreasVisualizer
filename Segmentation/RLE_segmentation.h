#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

#include "byteimage.h"

#pragma pack(push,1)
typedef struct SegmentedZone_ {
    Rect rect;
    size_t size;
} SegmentedZone;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct SegmentedZonesArray_ {
    struct SegmentedZone* zones;
    size_t numberOfElements;
} SegmentedZonesArray;
#pragma pack(pop)

enum PixelsConnectionType {CONNECTION_4 = 0, CONNECTION_8 = 1};

SegmentedZonesArray* createSegmentedZonesArray(size_t numberOfZones);
void releaseSegmentedZoneArray(SegmentedZonesArray** segmentedZonesArray);

int RleSegmentation(const ByteImage* image, PixelsConnectionType connectionType,
                    SegmentedZonesArray** zones);
