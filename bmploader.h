#ifndef BMPLOADER_H
#define BMPLOADER_H

#include "abstractloader.h"

/**
 * @brief Implementation of AbstractLoader.
 * @see AbstractLoader
 * Load record's frames that store as set of bmp images.
 */
class BmpLoader : public AbstractLoader
{
public:

    /**
     * @brief Obtains names of all bmp files stored in path.
     * @param path Directory where record stores in bmp format.
     */
    BmpLoader(const QString& path);

    /**
     * @copydoc AbstractLoader::loadFrame(int frameId)
     */
    QPixmap loadFrame(int frameId);

    /**
     * @copydoc AbstractLoader::size(void)
     */
    int size(void);

private:
    QVector<QString> mFileNames;
};

#endif // BMPLOADER_H
