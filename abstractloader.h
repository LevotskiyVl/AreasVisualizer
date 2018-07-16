#ifndef ABSTRACTLOADER_H
#define ABSTRACTLOADER_H

#include <memory>

#include <QPixmap>

/**
 * @brief This class provides interface for loading frames.
 * Each object stores path to record.
 * Inheritors must implement loadFrame method.
 */
class AbstractLoader
{
protected:
    QString mPath;
public:
    AbstractLoader(const QString& path);
    virtual ~AbstractLoader();

    /**
     * @brief This method should load record's frame specified by it's id.
     * @param frameId Unique identifier of frame.
     * @return Requested frame, represented as QPixmap.
     */
    virtual QPixmap loadFrame(int frameId) = 0;

    /**
     * @brief Allows to know how much frames in record.
     * @return Amount of frames in loaded record.
     */
    virtual int size(void) = 0;
};

#endif // ABSTRACTLOADER_H
