#include "main_window.h"
#include "ui_main_window.h"

#include "start_simple_render_command.h"
#include "color.h"

#include <QtConcurrent>
#include <QCloseEvent>

// #include <QDebug>
// #include <QColor>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
  setWindowTitle("Рейтрейсинг");

  ui->setupUi(this);
  ui->graphicsView->viewport()->setMinimumSize(MIN_SCENE_WIDTH,
                                               MIN_SCENE_HEIGHT);
  ui->graphicsView->setHorizontalScrollBarPolicy(
      Qt::ScrollBarAlwaysOff); // Отключаем скроллбары
  ui->graphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  QSize size = ui->graphicsView->viewport()->size();
  _qt_scene = std::make_shared<QGraphicsScene>(this);
  _pixmap = std::make_shared<QPixmap>(size);

  _drawer = std::make_shared<Drawer>(*_pixmap.get());
  _render_drawer = std::make_shared<Drawer>(*_pixmap.get());
  _scene = std::make_shared<Scene>();
  _render = std::make_shared<Render>();
  _facade = std::make_shared<Facade>();
  _world = _scene->draw();

  // _render->samples_per_pixel = 100;
  // _render->max_depth = 30;

  camera.vfov = 40;
  camera.lookfrom = point3(278, 278, -800);
  camera.lookat = point3(278, 278, 0);
  camera.vup = Vec3(0, 1, 0);
  camera.defocus_angle = 0;
  camera.focus_dist = 10;

  _render->set_camera(camera);

  connect(&_futureWatcher, &QFutureWatcher<void>::finished, this,
          &MainWindow::tile_render_finished_slot);

  set_scene();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  QWidget::resizeEvent(event);
  if (!_scene || !ui->graphicsView->scene() ||
      ui->graphicsView->scene()->items().isEmpty())
    return;

  _livetime_render();
}

void MainWindow::tile_render_finished_slot()
{
  if (!cancel_running)
  {
    _render_drawer->draw(*_render_color_matrix);
    update_render_scene();
  }
}

void MainWindow::set_scene()
{
  _qt_scene->setSceneRect(_pixmap.get()->rect());
  ui->graphicsView->setScene(_qt_scene.get());

  QGraphicsPixmapItem *pixmapItem = _qt_scene->addPixmap(*_pixmap.get());
}

void MainWindow::on_renderButton_clicked()
{
  cancel_running = false;

  _popup = new QWidget();
  _popup->setAttribute(Qt::WA_DeleteOnClose);
  connect(_popup, &QWidget::destroyed, this, &MainWindow::pop_up_closed_slot);

  ui->renderButton->setEnabled(false);
  QSize size = ui->graphicsView->viewport()->size();

  _render_color_matrix =
      std::make_shared<ColorMatrix>(size.height(), size.width());
  _render_pixmap = std::make_shared<QPixmap>(size);
  _render_drawer = std::make_shared<Drawer>(*_render_pixmap);
  _render_pixmap->fill(Qt::black);

  final_render_settings.samples_per_pixel = ui->renderSampleCount->value();
  final_render_settings.max_depth = ui->renderDepthCount->value();

  std::cout << final_render_settings.samples_per_pixel << " " << final_render_settings.max_depth << " " << final_render_settings.background << std::endl;
  _render->set_render_settings(final_render_settings);
  _futureWatcher.setFuture(QtConcurrent::run([this, size]()
                                             { _render->render(_world, *_render_color_matrix, cancel_running, 8, [this]()
                                                               { QMetaObject::invokeMethod(this, "tile_render_finished_slot",
                                                                                           Qt::QueuedConnection); }); }));

  _render_label = new QLabel(_popup);
  update_render_scene();
  _render_label->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

  QVBoxLayout *layout = new QVBoxLayout(_popup);
  layout->addWidget(_render_label, 0, Qt::AlignCenter);
  layout->setContentsMargins(0, 0, 0, 0);

  _popup->resize(size);
  _popup->show();
}

void MainWindow::on_changeWorldColor_clicked()
{
  // Получаем текущий цвет кнопки, если нужно (опционально)
  // QColor currentColor = ui->changeWorldCount->palette().Color(QPalette::Button);

  // Открываем диалог выбора цвета
  QColor selectedColor = QColorDialog::getColor(Qt::white, this, "Выберите цвет");

  // Проверяем, что пользователь не нажал "Отмена"
  if (selectedColor.isValid())
  {
    // Устанавливаем цвет кнопки через стилизацию
    QString style = QString("background-Color: rgb(%1, %2, %3);")
                        .arg(selectedColor.red())
                        .arg(selectedColor.green())
                        .arg(selectedColor.blue());
    ui->changeWorldColor->setStyleSheet(style);

    // Преобразуем компоненты в диапазон [0.0, 1.0]
    float r = selectedColor.redF(); // redF() возвращает float от 0.0 до 1.0
    float g = selectedColor.greenF();
    float b = selectedColor.blueF();
    float a = selectedColor.alphaF();

    final_render_settings.background = Color{r, g, b};
    live_render_settings.background = Color{r, g, b};

    _livetime_render();
    qDebug()
        << "Цвет кнопки (R G B):" << r << g << b << a;

    // ИЛИ, если нужно вывести в QLabel (например, ui->colorLabel):
    // ui->colorLabel->setText(QString("R: %1, G: %2, B: %3, A: %4")
    //     .arg(r, 0, 'f', 3)
    //     .arg(g, 0, 'f', 3)
    //     .arg(b, 0, 'f', 3)
    //     .arg(a, 0, 'f', 3));
  }
}
void MainWindow::pop_up_closed_slot()
{
  cancel_running = true;
  ui->renderButton->setEnabled(true);
  qDebug() << "destroy";
}

void MainWindow::update_render_scene()
{
  _render_label->setPixmap(*_render_pixmap.get());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  if (!is_mw_closed)
  {
    is_mw_closed = true;
    event->ignore();

    qDebug() << "MainWindow Destroy";

    // 1. Сигнализируем отмену
    cancel_running = true;
    QTimer::singleShot(50, this, [this]()
                       {
                         QTimer::singleShot(100, this, &MainWindow::close);
                         QMainWindow::close(); // вызов closeEvent
                       });
    return;
  }
  event->accept();
}

void MainWindow::_livetime_render()
{
  QSize new_size = ui->graphicsView->viewport()->size();

  _pixmap = std::make_shared<QPixmap>(new_size);
  _drawer = std::make_shared<Drawer>(*_pixmap);
  _color_matrix =
      std::make_shared<ColorMatrix>(new_size.height(), new_size.width());

  cancel_live_view_running = false;
  StartSimpleRenderCommand command{live_render_settings, camera, _world, *_color_matrix, cancel_live_view_running};
  _facade->execute(command);

  _drawer->draw(*_color_matrix);

  set_scene();
}

MainWindow::~MainWindow() {}
