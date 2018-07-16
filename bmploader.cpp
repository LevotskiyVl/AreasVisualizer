#include <QDirIterator>
#include <QImageReader>

#include "bmploader.h"

BmpLoader::BmpLoader(const QString& path)
    : AbstractLoader(path)
{
    if (!mPath.endsWith("/")) {
        mPath.append("/");
    }

    //obtain names of all bmp files in directory
    QDirIterator it(mPath, QStringList() << "*.bmp", QDir::Files);
    QVector<QFileInfo> fileInfos;
    while (it.hasNext()) {
        it.next();
        fileInfos.push_back(it.fileInfo());
    }

    //it should be ordered by asc
    std::sort(fileInfos.begin(), fileInfos.end(),
              [](QFileInfo a, QFileInfo b) {
           return a.baseName().toInt() < b.baseName().toInt();
    });

    //store filenames in QVector
    auto record = std::make_shared<QVector<QString>>();
    for (const auto& file : fileInfos) {
        mFileNames.push_back(file.fileName());
    }
}

QPixmap BmpLoader::loadFrame(int frameId)
{
    QPixmap frame;
    frame.load(mPath + mFileNames[frameId], "bmp", Qt::MonoOnly);
    return QPixmap::fromImage(frame.toImage().mirrored());
}

int BmpLoader::size(void)
{
    return mFileNames.size();
}
