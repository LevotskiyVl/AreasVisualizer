#include <QDebug>
#include <QDirIterator>
#include <QPainter>
#include "areashandler.h"

const size_t AREA_FIELDS_COUNT = 4;
const size_t AREA_FIELD_SIZE = 4;

AreasHandler::AreasHandler(const QString& path)
    : mPath(path)
{
    loadAreas();
}

void AreasHandler::loadAreas(void)
{
    //obtain names of all areas files in directory
    QDirIterator it(mPath, QStringList() << "areas_*", QDir::Files);
    QVector<QFileInfo> fileNames;
    while (it.hasNext()) {
        it.next();
        fileNames.push_back(it.fileInfo());
    }

    //it should be ordered by asc
    std::sort(fileNames.begin(), fileNames.end(), [](QFileInfo a, QFileInfo b) {
        return a.baseName().remove(0, 6).toInt()
                < b.baseName().remove(0, 6).toInt();
    });

    mAreasVectors.resize(fileNames.size());
    //for each frame
    for (int frameId(0); frameId < fileNames.size(); frameId++) {
        QFile currentAreasFile(fileNames[frameId].filePath());
        if (!currentAreasFile.open(QIODevice::ReadOnly)) {
            continue;
        }

        size_t areasAmount = (currentAreasFile.size()) >> 4;

        mAreasVectors[frameId].resize(areasAmount);
        uint32_t buffer[4];
        //read all areas that belong on this frame
        for (size_t areaId(0); areaId < areasAmount; areaId++) {
            currentAreasFile.read((char*)buffer,
                                  AREA_FIELDS_COUNT * 4);
            mAreasVectors[frameId][areaId].uplc.x = buffer[0];
            mAreasVectors[frameId][areaId].uplc.y = buffer[1];
            mAreasVectors[frameId][areaId].rect.h = buffer[2];
            mAreasVectors[frameId][areaId].rect.w = buffer[3];
        }
        currentAreasFile.close();
    }
}

bool AreasHandler::isEmpty(void)
{
    return mAreasVectors.size() == 0;
}

const QVector<gip_area>& AreasHandler::getFrameAreas(size_t frameId)
{
    return mAreasVectors[frameId];
}

gip_area AreasHandler::getArea(size_t frameId, size_t areaId){
    return mAreasVectors[frameId][areaId];
}

QPixmap AreasHandler::drawAreas(const QPixmap& source, size_t frameId)
{
    QPixmap frameWithAreas(source);
    QPainter painter(&frameWithAreas);
    QPen pen(Qt::DashLine);

    for(const auto area : mAreasVectors[frameId]) {
        pen.setColor(Qt::black);
        pen.setDashOffset(4.0);
        painter.setPen(pen);
        painter.drawRect(area.uplc.x,
        source.height() - area.uplc.y - area.rect.h, area.rect.w, area.rect.h);

        pen.setColor(Qt::white);
        pen.setDashOffset(0.0);
        painter.setPen(pen);
        painter.drawRect(area.uplc.x,
        source.height() - area.uplc.y - area.rect.h, area.rect.w, area.rect.h);
    }

    return frameWithAreas;
}

int AreasHandler::size(void)
{
    return mAreasVectors.size();
}
