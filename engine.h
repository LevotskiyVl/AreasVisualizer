#ifndef ENGINE_H
#define ENGINE_H
#include <memory>
#include <thread>

#include <QObject>

#include "bmploader.h"
#include "mainwindow.h"
#include "recordmanager.h"
#include "areashandler.h"
#include "recognizer.h"

/**
 * @brief This is the core class of this project.
 */
class Engine : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Construct Engine object after MainWindow, cause it need to use ui.
     * @param mUi Pointer to object that represents GUI.
     * @param win Engine would get signals from win.
     */
    Engine(Ui::MainWindow* mUi, MainWindow* win);
    ~Engine();

    /**
     * @brief Pass a frame to this method for further processing.
     * @param frame This frame should be processed.
     */
    void processFrame(const QPixmap& frame);

    /**
     * @brief Converts image from QPixmap to raw byte array.
     * @param src Image that should be converted.
     * @return Raw image data.
     */
    uint8_t* pixmapToBytes(const QPixmap& src);

    /**
     * @brief Converts image from raw byte array to QPixmap.
     * @param src Image that should be converted.
     * @param width Width of image in pixels.
     * @param height Height of image in pixels.
     * @return Image in QPixmap format.
     */
    QPixmap byteArrayToPixmap(const uint8_t* src, int width, int height);

private:
    explicit Engine(QObject* parent = nullptr);

    Ui::MainWindow* mUi;
    MainWindow* mWin;
    std::unique_ptr<RecordManager> mRecMngr;
    std::unique_ptr<AreasHandler> mAreasHndlr;
    std::unique_ptr<Recognizer> mRecognizer;
    int mDelay;

    void connectSignals(void);

private slots:
    void startPlayingRecord(void);
    void playRecord(void);
    void pauseRecord(void);
    void showNextFrame(void);
    void showPrevFrame(void);

    void loadRecord(void);
    void loadAreas(void);
    void saveFrames(const QString& path);
};

#endif // ENGINE_H
