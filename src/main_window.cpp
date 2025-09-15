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
    _drawer = std::make_shared<Drawer>(*_pixmap.get());

    set_scene();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    QSize new_size = ui->graphicsView->viewport()->size();
    _pixmap = std::make_shared<QPixmap>(new_size);
    _drawer = std::make_shared<Drawer>(*_pixmap.get());

    if (ui->graphicsView->scene() && !ui->graphicsView->scene()->items().isEmpty())
    {
        ui->graphicsView->fitInView(
            ui->graphicsView->scene()->items().first(),
            Qt::IgnoreAspectRatio);
        _drawer->main();
        set_scene();
    }
}

void MainWindow::set_scene()
{
    _scene->setSceneRect(_pixmap.get()->rect());
    ui->graphicsView->setScene(_scene.get());

    QGraphicsPixmapItem *pixmapItem = _scene->addPixmap(*_pixmap.get());
}