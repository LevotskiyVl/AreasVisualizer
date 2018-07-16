#ifndef RECOGNIZER_H
#define RECOGNIZER_H
#include <QByteArray>
#include <QPixmap>

#include "Recognizer/byteimage.h"
#include "Recognizer/motion_detector/motion.h"

/**
 * @brief Plate detecor and recognizer. Wrapped over C code
 */
class Recognizer
{
public:
    /**
     * @brief Recognizer инициализация распозавателя: инициализация настроек распознавателя,
     * загрузка шаблонов и нейронных сетей.
     */
    Recognizer(uint8_t *imageData, uint16_t width, uint16_t height);

    /**
     * @brief Recognizer освобождение ресурсов распозавателя: очистка детектора движения,
     * очистка шаблонов и НС
     */
    ~Recognizer();
    MotionAreaArray* getMotionAreas(const ByteImage* image);

};

#endif // RECOGNIZER_H
