#include "engine.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    Engine e(w.getUi(), &w);
    return a.exec();
}
