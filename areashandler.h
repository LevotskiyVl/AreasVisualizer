#ifndef AREASHANDLER_H
#define AREASHANDLER_H

#include "frame.h"
#include <QString>
#include <QVector>
#include <QPixmap>

/**
 * @brief Handles areas and actions for them (e.g. draw).
 */
class AreasHandler
{
public:

    /**
     * @brief Constructs new AreasHandler.
     * @param path Directory where areas files is storing.
     */
    AreasHandler(const QString& path);

    /**
     * @brief Checks, is it handles some areas or not.
     * @return true, if there is no area
     */
    bool isEmpty(void);

    /**
     * @brief Get all areas that belongs on this frame.
     * @param frameId Specifies frame.
     * @return Vector of areas for this frame.
     */
    const QVector<gip_area>& getFrameAreas(size_t frameId);

    /**
     * @brief Get specific area for frame.
     * @param frameId Specifies frame.
     * @param areaId Specifies area.
     * @return One area of frame.
     */
    gip_area getArea(size_t frameId, size_t areaId);

    /**
     * @brief Draw all areas on specified frame.
     * @param source Clean image.
     * @param frameId Unique identifier of frame.
     * @return New image with areas drew on.
     */
    QPixmap drawAreas(const QPixmap& source, size_t frameId);

    /**
     * @brief Allows to know how much areas is present.
     * @return Amount of areas on loaded record.
     */
    int size(void);

private:
    QString mPath;
    QVector<QVector<gip_area>> mAreasVectors;

    void loadAreas(void);
};

#endif // AREASHANDLER_H
