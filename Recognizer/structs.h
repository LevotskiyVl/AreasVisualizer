#ifndef STRUCTS_H
#define STRUCTS_H

//Модуль содержит описание базовых структур данных

#include <stdlib.h>
#include <stdint.h>

typedef struct Rect_{
    uint16_t x, y, width, height;
}Rect;

bool isRectInside(Rect* rect1, Rect* rect2, uint16_t offset);
Rect mergeRects(Rect* rect1, Rect* rect2);

#endif // STRUCTS_H
