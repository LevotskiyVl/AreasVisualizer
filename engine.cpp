#include <QFile>
#include <QTimer>
#include <QBuffer>
#include <iostream>

#include "engine.h"
#include "ui_mainwindow.h"

#include "Recognizer/byteimage.h"
#include "Recognizer/motion_detector/motion.h"

const int MSEC_PER_SEC = 1000;

Engine::Engine(Ui::MainWindow* ui, MainWindow* win)
    : mUi(ui), mWin(win)
{
    mDelay = MSEC_PER_SEC / ui->fpsSpinBox->text().toInt();
    connectSignals();
}

Engine::Engine(QObject* parent) : QObject(parent)
{

}

void Engine::processFrame(const QPixmap& frame)
{
    uint8_t* bytes = pixmapToBytes(frame);
    ByteImage byteImage;
    int error = createByteImageHeader(frame.width(), frame.height(), 1, bytes, &byteImage);
    if (error) { printError(error); return; }

    MotionAreaArray* motionFrames = mRecognizer->getMotionAreas(&byteImage);
    std::cout << motionFrames->numberOfElements << std::endl;

}

uint8_t* Engine::pixmapToBytes(const QPixmap& src)
{
    return src.toImage().bits();
}

QPixmap Engine::byteArrayToPixmap(const uint8_t* src,
                              int width, int height)
{
    QByteArray byteArray = QByteArray::fromRawData((const char*)src,
                                                   width * height);
    QDataStream pixstream(&byteArray, QIODevice::ReadOnly);
    QPixmap pixmap(width, height);
    pixstream >> pixmap;
    return pixmap;
}

void Engine::connectSignals(void)
{
    QObject::connect(mWin, SIGNAL(requestNextFrame(void)),
                     SLOT(showNextFrame(void)));
    QObject::connect(mWin, SIGNAL(requestPrevFrame(void)),
                     SLOT(showPrevFrame(void)));
    QObject::connect(mWin, SIGNAL(requestStart(void)),
                     SLOT(startPlayingRecord(void)));
    QObject::connect(mWin, SIGNAL(requestPause(void)),
                     SLOT(pauseRecord(void)));

    QObject::connect(mWin, SIGNAL(recordPathEditingFinished(void)),
                     SLOT(loadRecord(void)));
    QObject::connect(mWin, SIGNAL(areasPathEditingFinished(void)),
                     SLOT(loadAreas(void)));
    QObject::connect(mWin, SIGNAL(savingPathEditingFinished(const QString&)),
                     SLOT(saveFrames(const QString&)));

    QObject::connect(mWin, &MainWindow::fpsChanged, this, [this](int fps) {
        mDelay = MSEC_PER_SEC / fps;
    });
}

void Engine::startPlayingRecord(void)
{
    mRecMngr->play();
    mUi->pauseButton->setEnabled(true);

    //prevent unnecessary pushing
    mUi->playButton->setEnabled(false);
    mUi->nextButton->setEnabled(false);
    mUi->prevButton->setEnabled(false);

    //show first frame
    if (mAreasHndlr.get() == nullptr ||
            mRecMngr->getCurrentFrameId() > mAreasHndlr->size() - 1) {
        mUi->videoLabel->setPixmap(mRecMngr->current());
    }
    else {
        QPixmap curFrame = mRecMngr->current();
        mUi->videoLabel->setPixmap(mAreasHndlr->drawAreas(curFrame,
                                        mRecMngr->getCurrentFrameId()));
    }
    mUi->videoLabel->repaint();
    playRecord();
}

void Engine::playRecord(void)
{
    if (mRecMngr->isPlaying() && mRecMngr->getCurrentFrameId() <
            mRecMngr->getRecordSize()) {
        showNextFrame();
        mUi->videoLabel->repaint();

        QTimer::singleShot(mDelay, this, SLOT(playRecord()));
    }
}

void Engine::pauseRecord(void)
{
    mRecMngr->pause();
    mUi->playButton->setEnabled(true);
    mUi->nextButton->setEnabled(true);
    mUi->prevButton->setEnabled(true);
}

void Engine::showNextFrame(void)
{
    if (mRecMngr.get() == nullptr) {
        return;
    }


    if (mAreasHndlr.get() == nullptr ||
            mRecMngr->getCurrentFrameId() >= mAreasHndlr->size() - 1) {
        QPixmap nextFrame = mRecMngr->next();
        mUi->videoLabel->setPixmap(nextFrame);
    }
    else {
        QPixmap nextFrame = mRecMngr->next();
        mUi->videoLabel->setPixmap(mAreasHndlr->drawAreas(nextFrame,
                                        mRecMngr->getCurrentFrameId()));
    }
}

void Engine::showPrevFrame(void)
{
    if (mRecMngr.get() == nullptr) {
        return;
    }

    if (mAreasHndlr.get() == nullptr ||
            mRecMngr->getCurrentFrameId() > mAreasHndlr->size()) {
        mUi->videoLabel->setPixmap(mRecMngr->prev());
    }
    else {
        QPixmap prevFrame = mRecMngr->prev();
        mUi->videoLabel->setPixmap(mAreasHndlr->drawAreas(prevFrame,
                                        mRecMngr->getCurrentFrameId()));
    }
}

void Engine::loadRecord(void)
{
    const QString& recordPath = mUi->recordPathSelectorLineEdit->text();
    if (recordPath.isEmpty()) {
        return;
    }

    //clean old record
    if (mRecMngr.get() != nullptr) {
        delete mRecMngr.release();
    }

    //setup record manager
    mRecMngr = std::make_unique<RecordManager>(recordPath);

    //unsuccessful loading
    if (mRecMngr->isEmpty()) {
        delete mRecMngr.release();
        return;
    }

    //recognizer initialization. An exception will be thrown if the initialization wasn't successful
    QPixmap firstImage = mRecMngr->current();
    mRecognizer = std::make_unique<Recognizer>(pixmapToBytes(firstImage), firstImage.width(), firstImage.height());

    QObject::connect(mRecMngr.get(), SIGNAL(lastFrame(void)),
                     SLOT(pauseRecord(void)));
    mUi->playButton->setEnabled(true);
    mUi->saveButton->setEnabled(true);
    mUi->nextButton->setEnabled(true);
    mUi->prevButton->setEnabled(true);
}

void Engine::loadAreas(void)
{
    //load areas if path to them is present
    const QString& areasPath = mUi->areasPathSelectorLineEdit->text();
    if (areasPath.isEmpty()) {
        return;
    }

    //clean old areas
    if (mAreasHndlr.get() != nullptr) {
        delete mAreasHndlr.release();
    }

    mAreasHndlr = std::make_unique<AreasHandler>(areasPath);
    if (mAreasHndlr->isEmpty()) {
        delete mAreasHndlr.release();
    }
}

void Engine::saveFrames(const QString& path)
{
    if (path.isEmpty() || mAreasHndlr.get() == nullptr) {
        return;
    }

    std::thread saver([path, this](){
        //prevent loading new record and areas while saving
        mUi->recordPathSelectorLineEdit->setEnabled(false);
        mUi->recordPathSelectorToolButton->setEnabled(false);
        mUi->areasPathSelectorLineEdit->setEnabled(false);
        mUi->areasPathSelectorToolButton->setEnabled(false);
        mUi->saveButton->setEnabled(false);

        //make local record manager so as not to interfere with the main thread
        RecordManager recMngr(mUi->recordPathSelectorLineEdit->text());

        //save only frames which have corresponding areas
        for (int i(0); i < mAreasHndlr->size(); i++) {
            QFile file(path + "/" + QString::number(i));
            file.open(QIODevice::WriteOnly);

            mAreasHndlr->drawAreas(recMngr.current(),
                        recMngr.getCurrentFrameId()).save(&file, "bmp");
            recMngr.next();
        }

        //release blocked widgets
        mUi->recordPathSelectorLineEdit->setEnabled(true);
        mUi->recordPathSelectorToolButton->setEnabled(true);
        mUi->areasPathSelectorLineEdit->setEnabled(true);
        mUi->areasPathSelectorToolButton->setEnabled(true);
        mUi->saveButton->setEnabled(true);
    });

    //let thread works independently
    saver.detach();
}

Engine::~Engine()
{

}
