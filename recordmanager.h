#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include <memory>

#include <QObject>
#include <QPixmap>
#include <QVector>

#include "bmploader.h"

/**
 * @brief Manages record playing.
 * Presents set of methods to proper playing of record, such as:
 * play, next, previous, etc.
 */
class RecordManager : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Initializes object that manages record stored in path.
     * @param path Directory or file with record (it depends on format).
     */
    RecordManager(const QString& path);
    ~RecordManager();

    /**
     * @brief Sets manager to active state.
     */
    void play(void);

    /**
     * @brief Allows to get current frame.
     * @return Current frame.
     */
    QPixmap current(void);

    /**
     * @brief Allows to get next frame.
     * @return Next frame or last, if end is reached.
     */
    QPixmap next(void);

    /**
     * @brief Allows to get previous frame.
     * @return Previous frame or first, if beginning is reached.
     */
    QPixmap prev(void);

    /**
     * @brief Sets position to first frame.
     */
    void goToFirst(void);

    /**
     * @brief Sets position to last frame.
     */
    void goToLast(void);

    /**
     * @brief Sets position to specified frame.
     */
    void goTo(int frameId);

    /**
     * @brief Sets manager to idle state.
     */
    void pause(void);

    /**
     * @copydoc AbstractLoader::size(void)
     */
    int getRecordSize(void);

    /**
     * @return Id of current frame.
     */
    int getCurrentFrameId(void);

    /**
     * @brief Check manager state using this method.
     * @return true if active
     */
    bool isPlaying(void);

    /**
     * @brief Check is it manages some record or not.
     * @return true if there is no frames
     */
    bool isEmpty(void);

private:
    explicit RecordManager(QObject *parent = nullptr);

    std::unique_ptr<AbstractLoader> mLoader;
    int mCurrentFrameId;
    bool mIsPlaying;

signals:
    void lastFrame(void);
};

#endif // RECORDMANAGER_H
