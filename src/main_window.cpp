// #include "main_window.h"
// #include "ui_main_window.h"

// #include <iostream>
// #include <QHBoxLayout>
// #include <QLabel>
// #include <QPixmap>
// #include <QWidget>

// MainWindow::MainWindow(QWidget *parent)
//     : QMainWindow(parent), ui(new Ui::MainWindow)
// {
//     ui->setupUi(this);


//     // this->setFixedSize(WINDOWS_X, WINDOWS_Y);
//     // create_scene()
// }

// // void MainWindow::create_scene()
// // {
// //     scene = new QGraphicsScene(ui->graphic_widget);

// //     ui->graphic_widget->setScene(scene);
// //     ui->graphic_widget->setSceneRect(0, 0, SCENE_SIZE, SCENE_SIZE);
// //     ui->graphic_widget->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
// //     ui->graphic_widget->setRenderHint(QPainter::Antialiasing);
// //     ui->graphic_widget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
// //     ui->graphic_widget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

// //     ui->graphic_widget->setAttribute(Qt::WA_TransparentForMouseEvents);
// //     ui->graphic_widget->installEventFilter(this);

// //     // Устанавливаем значения оффсета зоны для рисования
// //     QPoint pos_relative_to_window = ui->graphic_widget->mapTo(this, QPoint(0, 0));
// //     offset_x = pos_relative_to_window.x();
// //     offset_y = pos_relative_to_window.y();

// //     update_scene();
// // }

// MainWindow::~MainWindow(void)
// {
//     delete ui;
// }
