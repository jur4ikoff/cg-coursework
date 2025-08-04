#pragma once

#include "ui_main_window.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QWidget>

#define DEFAULT_WIDTH 1200
#define DEFAULT_HEIGHT 800

#define MIN_SCENE_WIDTH 700
#define MIN_SCENE_HEIGHT 600
QT_BEGIN_NAMESPACE

namespace Ui
{
class MainWindow;
}

QT_END_NAMESPACE

// написать GraphicsView и 
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr)
        : QMainWindow(parent), ui(new Ui::MainWindow)
    {
        ui->setupUi(this); // Загрузка UI-формы (обязательно!)

        // Настройка главного окна
        setWindowTitle("Фиксированный QPixmap слева (C++)");

        ui->imageLabel->setMinimumSize(MIN_SCENE_WIDTH, MIN_SCENE_HEIGHT); // Фиксированный размер
        this->ui->imageLabel->setAlignment(Qt::AlignCenter);

    }

private:
    Ui::MainWindow *ui;
};
