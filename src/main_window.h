#ifndef MAINWINDOW_H__
#define MAINWINDOW_H__

#include <QMainWindow>
#include <iostream>

#define WINDOWS_X 1400
#define WINDOWS_Y 750

#define SCENE_SIZE 700

QT_BEGIN_NAMESPACE

namespace Ui
{
    class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow(void);

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H__
