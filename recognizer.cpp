#include "recognizer.h"
#include "Recognizer/Recognizer_errors.h"
#include <QBuffer>


Recognizer::Recognizer(uint8_t* imageData, uint16_t width, uint16_t height)
{
    ByteImage* byteImage;
    int error = createByteImageHeader(width, height, 1, imageData, byteImage);
    if (error) {
        throw std::runtime_error("Recognizer initialization error: " + std::to_string(error));
    }
    error = initMovementDetector(byteImage, NULL);
    if (error) {
        throw std::runtime_error("Recognizer initialization error: " + std::to_string(error));
    }
}

Recognizer::~Recognizer()
{
    releaseMovementDetector();
}

MotionAreaArray* Recognizer::getMotionAreas(const ByteImage* image)
{
    MotionAreaArray* result = NULL;
    int error = motionDetector(image, &result);
    if (error) {
        throw std::runtime_error("Motion detector error: " + std::to_string(error));
    }
    return result;
}




