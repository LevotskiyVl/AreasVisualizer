#include <QFileDialog>
#include <QShortcut>
#include <QTextStream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connectSignals();
    new QShortcut(QKeySequence(Qt::Key_Right), this,
                  SIGNAL(requestNextFrame(void)));
    new QShortcut(QKeySequence(Qt::Key_Left), this,
                  SIGNAL(requestPrevFrame(void)));

    QFile pathcache(".pathcache");
    if (pathcache.open(QIODevice::ReadOnly)) {
        QTextStream stream(&pathcache);

        QString cachedPath = stream.readLine();
        if (!cachedPath.isEmpty()) {
            ui->recordPathSelectorLineEdit->setText(cachedPath);
        }

        cachedPath = stream.readLine();
        if (!cachedPath.isEmpty()) {
            ui->areasPathSelectorLineEdit->setText(cachedPath);
        }

        pathcache.close();
    }
}

void MainWindow::connectSignals(void)
{
    QObject::connect(ui->recordPathSelectorToolButton, SIGNAL(clicked(bool)),
                     SLOT(selectRecordPath(void)));
    QObject::connect(ui->recordPathSelectorLineEdit,
                     SIGNAL(editingFinished(void)),
                     SIGNAL(recordPathEditingFinished(void)));

    QObject::connect(ui->areasPathSelectorToolButton, SIGNAL(clicked(bool)),
                     SLOT(selectAreasPath(void)));
    QObject::connect(ui->areasPathSelectorLineEdit,
                     SIGNAL(editingFinished(void)),
                     SIGNAL(areasPathEditingFinished(void)));

    QObject::connect(ui->saveButton, SIGNAL(clicked(bool)),
                     SLOT(selectSavingPath(void)));

    QObject::connect(ui->playButton, SIGNAL(clicked(bool)),
                     SIGNAL(requestStart(void)));
    QObject::connect(ui->pauseButton, SIGNAL(clicked(bool)),
                     SIGNAL(requestPause(void)));
    QObject::connect(ui->nextButton, SIGNAL(clicked(bool)),
                     SIGNAL(requestNextFrame(void)));
    QObject::connect(ui->prevButton, SIGNAL(clicked(bool)),
                     SIGNAL(requestPrevFrame(void)));

    QObject::connect(ui->fpsSpinBox, SIGNAL(valueChanged(int)),
                     SIGNAL(fpsChanged(int)));
}

void MainWindow::selectRecordPath(void)
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                    "Открыть запись",
                                    getenv("HOME"),
                                    QFileDialog::ShowDirsOnly
                                    | QFileDialog::DontResolveSymlinks);
    ui->recordPathSelectorLineEdit->setText(dir);
    emit recordPathEditingFinished();
}

void MainWindow::selectAreasPath(void)
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                    "Открыть сохраненные рамки",
                                    getenv("HOME"),
                                    QFileDialog::ShowDirsOnly
                                    | QFileDialog::DontResolveSymlinks);
    ui->areasPathSelectorLineEdit->setText(dir);
    emit areasPathEditingFinished();
}

void MainWindow::selectSavingPath(void)
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                    "Выбрать директорию для сохранения кадров",
                                    getenv("HOME"),
                                    QFileDialog::ShowDirsOnly
                                    | QFileDialog::DontResolveSymlinks);
    emit savingPathEditingFinished(dir);
}

Ui::MainWindow* MainWindow::getUi(void)
{
    return ui;
}

MainWindow::~MainWindow()
{
    QFile pathcache(".pathcache");
    if (pathcache.open(QIODevice::WriteOnly)) {
        QTextStream stream(&pathcache);
        stream << ui->recordPathSelectorLineEdit->text() << "\n";
        stream << ui->areasPathSelectorLineEdit->text() << "\n";
        pathcache.close();
    }
    delete ui;
}
