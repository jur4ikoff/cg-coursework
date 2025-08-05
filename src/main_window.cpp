#include "main_window.h"
#include "ui_main_window.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->graphicsView->viewport()->setMinimumSize(MIN_SCENE_WIDTH, MIN_SCENE_HEIGHT);

    setWindowTitle("Фиксированный QPixmap слева (C++)");

    QGraphicsScene *scene = new QGraphicsScene(this);
    ui->graphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff); // Отключаем скроллбары
    ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QPixmap pixmap(ui->graphicsView->viewport()->size());
    pixmap.fill(Qt::red);

    scene->setSceneRect(pixmap.rect());
    ui->graphicsView->setScene(scene);

    QGraphicsPixmapItem *pixmapItem = scene->addPixmap(pixmap);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);

    if (ui->graphicsView->scene() && !ui->graphicsView->scene()->items().isEmpty())
    {
        ui->graphicsView->fitInView(
            ui->graphicsView->scene()->items().first(),
            Qt::IgnoreAspectRatio);
    }
}
