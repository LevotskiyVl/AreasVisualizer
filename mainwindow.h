#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLineEdit>

namespace Ui {
class MainWindow;
}

/**
 * @brief Represents GUI.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    Ui::MainWindow* getUi(void);
    ~MainWindow();

private:
    Ui::MainWindow* ui;

    void connectSignals(void);

public slots:
    void selectRecordPath(void);
    void selectAreasPath(void);
    void selectSavingPath(void);

signals:
    void requestNextFrame(void);
    void requestPrevFrame(void);
    void requestStart(void);
    void requestPause(void);
    void recordPathEditingFinished(void);
    void areasPathEditingFinished(void);
    void savingPathEditingFinished(const QString&);
    void fpsChanged(int);
};

#endif // MAINWINDOW_H
