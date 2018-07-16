#include "structs.h"

bool isRectInside(Rect* rect1, Rect* rect2, uint16_t offset)
{
    int16_t x1_i = rect1->x;
    int16_t y1_i = rect1->y;
    int16_t x2_i = rect1->x + rect1->width - 1;
    int16_t y2_i = rect1->y + rect1->height - 1;

    int16_t x1_j = rect2->x;
    int16_t y1_j = rect2->y;
    int16_t x2_j = rect2->x + rect2->width - 1;
    int16_t y2_j = rect2->y + rect2->height - 1;

    if ((    x1_i >= x1_j - offset && y1_i >= y1_j - offset)
         && (x2_i <= x2_j + offset && y2_i <= y2_j + offset)){
        return true;
    }
    if ((    x1_i >= x1_j - offset && y2_i >= y1_j - offset && y2_i <= y2_j + offset)
         && (x2_i <= x2_j + offset)){
        return true;
    }

    if ((    x1_i >= x1_j - offset && y1_i <= y1_j + offset && y2_i >= y2_j - offset)
         && (x2_i <= x2_j + offset)){
        return true;
    }

    return false;
}

Rect mergeRects(Rect* rect1, Rect* rect2)
{
    Rect result = *rect2;
    uint16_t x2_j = rect2->x + rect2->width - 1;
    uint16_t y2_j = rect2->y + rect2->height - 1;

    uint16_t x1_j = rect1->x + rect1->width - 1;
    uint16_t y1_j = rect1->y + rect1->height - 1;

    if (rect1->x < rect2->x) result.x = rect1->x;
    if (rect1->y < rect2->y) result.y = rect1->y;
    if (x1_j > x2_j) x2_j = x1_j;
    if (y1_j > y2_j) y2_j = y1_j;

    result.width = x2_j - result.x + 1;
    result.height = y2_j - result.y + 1;
    return result;
}

