#ifndef SETTINGS_H
#define SETTINGS_H

//Модуль настроек распознавателя.

//Настройки детектора движения
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
#define NESSTINGS_OFFSET_FOR_MOTION 3

//Настройки для RLE-сегментации
#define MAX_FULL_HD_OBJTS 518400
#endif // SETTINGS_H
