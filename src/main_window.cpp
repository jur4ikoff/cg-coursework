#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    setWindowTitle("Рейтрейсинг");

    ui->setupUi(this);
    ui->graphicsView->viewport()->setMinimumSize(MIN_SCENE_WIDTH, MIN_SCENE_HEIGHT);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключаем скроллбары
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    _scene = std::make_shared<QGraphicsScene>(this);
    _pixmap = std::make_shared<QPixmap>(ui->graphicsView->viewport()->size());
    _graphic_scene = std::make_shared<QtDrawer>(*_pixmap.get());
    _raytraicer = std::make_shared<RayTraicer>();

    set_scene();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    QSize new_size = ui->graphicsView->viewport()->size();
    _pixmap = std::make_shared<QPixmap>(new_size);
    _graphic_scene = std::make_shared<QtDrawer>(*_pixmap.get());

    if (ui->graphicsView->scene() && !ui->graphicsView->scene()->items().isEmpty())
    {
        ui->graphicsView->fitInView(
            ui->graphicsView->scene()->items().first(),
            Qt::IgnoreAspectRatio);

        ColorMatrix color_matrix = _raytraicer->draw(new_size);
        _graphic_scene->draw(color_matrix);
        set_scene();
    }
}

void MainWindow::set_scene()
{
    _scene->setSceneRect(_pixmap.get()->rect());
    ui->graphicsView->setScene(_scene.get());

    QGraphicsPixmapItem *pixmapItem = _scene->addPixmap(*_pixmap.get());
}