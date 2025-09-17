#pragma once

#include "ui_main_window.h"
#include "raytraicer.h"
#include "qt_drawer.h"
#include "color_matrix.h"

#include <QApplication>

#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QWidget>

#define DEFAULT_WIDTH 1200
#define DEFAULT_HEIGHT 800

#define MIN_SCENE_WIDTH 600
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
    MainWindow(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    std::shared_ptr<QtDrawer> _graphic_scene;
    std::shared_ptr<RayTraicer> _raytraicer;
    std::shared_ptr<QGraphicsScene> _scene;
    std::shared_ptr<QPixmap> _pixmap;
    Ui::MainWindow *ui;

    // QTimer _resize_timer;

    void set_scene();
};
