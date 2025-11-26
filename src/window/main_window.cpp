#include "main_window.h"
#include "ui_main_window.h"
#include <QtConcurrent>

// TODO segfault когда нажимаешь на крестик в попапе, выполнение все еще
// продолжается

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
  _scene = std::make_shared<Scene>();
  _render = std::make_shared<Render>();
  _world = _scene->draw();

  _render->samples_per_pixel = 100;
  _render->max_depth = 20;
  _render->background = color(0, 0, 0);

  Camera camera;
  camera.vfov =40;
  camera.lookfrom = point3(278, 278, -800);
  camera.lookat = point3(278, 278, 0);
  camera.vup = vec3(0, 1, 0);
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

  QSize new_size = ui->graphicsView->viewport()->size();

  _pixmap = std::make_shared<QPixmap>(new_size);
  _pixmap->fill(Qt::black);

  _drawer = std::make_shared<Drawer>(*_pixmap.get());
  _color_matrix =
      std::make_shared<ColorMatrix>(new_size.height(), new_size.width());

  set_scene();
}

void MainWindow::tile_render_finished_slot()
{
  if (!cancel_running)
  {
    // qDebug() << "draw";
    _drawer->draw(*_render_color_matrix);
    update_render_scene();
  }
}

void MainWindow::pop_up_closed_slot()
{
  cancel_running = true;
  ui->renderButton->setEnabled(true);
  qDebug() << "destroy";
}

void MainWindow::set_scene()
{
  _qt_scene->setSceneRect(_pixmap.get()->rect());
  ui->graphicsView->setScene(_qt_scene.get());

  QGraphicsPixmapItem *pixmapItem = _qt_scene->addPixmap(*_pixmap.get());
}

void MainWindow::on_renderButton_clicked()
{
  _popup = new QWidget();
  _popup->setAttribute(Qt::WA_DeleteOnClose);
  connect(_popup, &QWidget::destroyed, this, &MainWindow::pop_up_closed_slot);

  ui->renderButton->setEnabled(false);
  QSize size = ui->graphicsView->viewport()->size();

  _render_color_matrix =
      std::make_shared<ColorMatrix>(size.height(), size.width());
  _render_pixmap = std::make_shared<QPixmap>(size);
  _drawer = std::make_shared<Drawer>(*_render_pixmap);
  _render_pixmap->fill(Qt::black);

  cancel_running = false;
  qDebug() << size;
  std::cout << _render_color_matrix->size() << std::endl;
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

void MainWindow::update_render_scene()
{
  _render_label->setPixmap(*_render_pixmap.get());
}

void MainWindow::disable_render_button() {}

MainWindow::~MainWindow() {}
