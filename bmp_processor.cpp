#include "bmp_processor.h"
#include <string.h>

int readBmpFileHeader(FILE* stream, FILEHEADER* dst)
{
    if (dst == NULL || stream == NULL) { return NULL_PTR_ERROR;}
    size_t readedBytes = fread(dst, 1, sizeof(FILEHEADER), stream);
    if (readedBytes != sizeof(FILEHEADER)) { return FILE_READING_ERROR; }
    return 0;
}

int readBmpInfoHeader(FILE* stream, INFOHEADER* dst)
{
    if (dst == NULL || stream == NULL) { return NULL_PTR_ERROR;}
    size_t readedBytes = fread(dst, 1, sizeof(INFOHEADER), stream);
    if (readedBytes != sizeof(INFOHEADER)) { return FILE_READING_ERROR; }
    return 0;
}

void readBmpPixelArray(size_t width, size_t height, uint8_t bytesOnPixel,
                       size_t padding, FILE *stream, uint8_t* dst)
{
    //Инициализация памяти для чтения дополнения строк
    uint8_t dummy[10];
    uint16_t bytesInRow = width * bytesOnPixel  * sizeof(uint8_t);
    size_t image_size = height * bytesInRow;
    uint8_t* next = dst, *last = dst + image_size;
    //чтение пикселей
    if (padding == 0) {
        fread(next, sizeof(uint8_t), image_size, stream);
    }
    else{
        while (next != last) {
            fread(next,  sizeof(uint8_t), bytesInRow, stream);
            fread(dummy, sizeof(uint8_t), padding, stream);
            next += bytesInRow;
        }
    }
}

int readBmpImage(const char* path, ByteImage* img)
{
    if (path == NULL || img == NULL) { return NULL_PTR_ERROR; }
    FILEHEADER fileheader;
    INFOHEADER infoheader;
    FILE* f = fopen(path, "r");
    if (f == NULL) { return WRONG_PATH; }
    int error = readBmpFileHeader(f, &fileheader);
    if (error) { return error; }
    error = readBmpInfoHeader(f, &infoheader);
    if (error) { return error; }

    img->height = abs(infoheader.biHeight);
    img->width = abs(infoheader.biWidth);
    img->numberOfChannels = infoheader.biBitCount / 8;
    img->widthStep = img->width * img->numberOfChannels * sizeof(uint8_t);
    img->ROI.height = img->height;
    img->ROI.width = img->width;
    img->ROI.xOffset = 0;
    img->ROI.yOffset = 0;
    img->data = (uint8_t*) malloc(img->height * img->width * img->numberOfChannels * sizeof(uint8_t));
    if (img->data == NULL) { return ALLOCATION_ERROR; }

    fseek(f, fileheader.bfOffBits, SEEK_SET);
    readBmpPixelArray(img->width, img->height, img->numberOfChannels, (img->width * 3) % 4, f, img->data);

    fclose(f);
    return 0;
}

void fillBmpFileHeader(const ByteImage* img, FILEHEADER* header)
{
    int w = img->width;
    int h = img->height;
    header->bfType = BM_CODE;
    header->Reserved1 = 0;
    header->Reserved2 = 0;
    header->bfSize = sizeof(uint8_t) * w * h * img->numberOfChannels
             + sizeof(FILEHEADER) + sizeof(FILEHEADER)+ COLOR_PIXELS_SIZE;
    header->bfOffBits = sizeof(FILEHEADER) + sizeof(INFOHEADER) + COLOR_PIXELS_SIZE;
}

void fillBmpInfoHeader(const ByteImage* img, INFOHEADER* info)
{
    int w = img->width;
    int h = img->height;
    info->biSize = sizeof(INFOHEADER);
    info->biWidth = w;
    info->biHeight = h;
    info->biPlanes = 1;
    info->biBitCount = sizeof(uint8_t) * 8 * img->numberOfChannels;
    info->biCompression = 0;
    info->biSizeImage = w * h * img->numberOfChannels * sizeof(uint8_t);
    info->biXPelsPerMeter = XPelsPerMeter;
    info->biYPelsPerMeter = YPelsPerMeter;
    info->biClrUsed = 256;
    info->biClrImportant = 256;
}

void saveBmpHeader(const ByteImage* image, FILE* stream)
{

    FILEHEADER fileheader;
    INFOHEADER infoheader;

    fillBmpFileHeader(image, &fileheader);
    fillBmpInfoHeader(image, &infoheader);

    uint8_t* head = (uint8_t*) malloc(fileheader.bfOffBits * sizeof(uint8_t));
    uint8_t pixels[COLOR_PIXELS_SIZE];
    for (int i =0; i < 256;i++) {
        const int num = i * 4;
        for (int j = 0; j < 3; j++) {
            pixels[num + j] = i;
        }
        pixels[num+3]=0;
    }
    memcpy(head, (void*)&fileheader, sizeof(FILEHEADER));
    memcpy(head+sizeof(FILEHEADER), (void*)&infoheader, sizeof(INFOHEADER));
    memcpy(head+sizeof(FILEHEADER) + sizeof(INFOHEADER), (void*)pixels, COLOR_PIXELS_SIZE);

    fseek(stream,0, SEEK_SET);
    fwrite((void*)head, sizeof(uint8_t), fileheader.bfOffBits, stream);
    free(head);
}

void saveBmpPixelArray(const ByteImage* image, FILE* stream)
{
    uint8_t* src = image->data;
    size_t width = image->width;
    size_t height = image->height;
    uint8_t bytesOnPixel = image->numberOfChannels * sizeof(uint8_t);
    uint8_t widthStep = image->widthStep;
    size_t padding = width * 3 % 4;

    //Initializing dummy memory for reading padding bytes.
    uint8_t* dummy = (uint8_t*)calloc(padding, sizeof(uint8_t));
    fseek(stream, 1078, SEEK_SET);
    if (padding == 0) {
        fwrite(src,  sizeof(uint8_t), width * bytesOnPixel * height, stream);
    }
    else{
        for (size_t i = 0; i < height; i++) {
            fwrite(&src[i * widthStep],  sizeof(uint8_t), width * bytesOnPixel, stream);
            fwrite(dummy, sizeof(uint8_t), padding, stream);
        }
    }
    free(dummy);
}

int saveBmpImage(const char* path, const ByteImage* img)
{
    FILE* file = fopen(path, "w");
    if (file == NULL) { return WRONG_PATH; }

    saveBmpHeader(img, file);
    saveBmpPixelArray(img, file);

    fclose(file);
    return 0;
}

