#include "recordmanager.h"

RecordManager::RecordManager(QObject *parent) : QObject(parent)
{

}

RecordManager::RecordManager(const QString& path)
    : mCurrentFrameId(0), mIsPlaying(false)
{
    mLoader = std::make_unique<BmpLoader>(path);
}

void RecordManager::play(void)
{
    mIsPlaying = true;
}

QPixmap RecordManager::current(void)
{
    return mLoader->loadFrame(mCurrentFrameId);
}

QPixmap RecordManager::next(void)
{
    if (mCurrentFrameId < getRecordSize() - 1) {
        mCurrentFrameId++;
    } else {
        emit lastFrame();
    }
    return current();
}

QPixmap RecordManager::prev(void)
{
    if (mCurrentFrameId > 0) {
        mCurrentFrameId--;
    }
    return current();
}

void RecordManager::goToFirst(void)
{
    mCurrentFrameId = 0;
}

void RecordManager::goToLast(void)
{
    mCurrentFrameId = getRecordSize() - 1;
}

void RecordManager::goTo(int frameId)
{
    mCurrentFrameId = frameId;
}

void RecordManager::pause(void)
{
    mIsPlaying = false;
}

int RecordManager::getRecordSize(void)
{
    return mLoader->size();
}

int RecordManager::getCurrentFrameId(void)
{
    return mCurrentFrameId;
}

bool RecordManager::isPlaying(void)
{
    return mIsPlaying;
}

bool RecordManager::isEmpty(void)
{
    return mLoader->size() == 0;
}

RecordManager::~RecordManager()
{

}
