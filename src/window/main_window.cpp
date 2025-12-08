#include "main_window.h"
#include "ui_main_window.h"

#include "object_add_dialog.h"
#include "power_color_dialog.h"
#include "material_dialog.h"
#include "move_object_dialog.h"
#include "rotate_object_dialog.h"
#include "camera_add.h"

#include "color.h"
#include "start_simple_render_command.h"

#include <QCloseEvent>
#include <QDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QtConcurrent>

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

  init_camera();
  init_scene();

  connect(&_futureWatcher, &QFutureWatcher<void>::finished, this,
          &MainWindow::tile_render_finished_slot);

  set_scene();
}

void MainWindow::init_camera()
{
  auto camera = std::make_shared<Camera>();
  camera->id = 0;
  camera->vfov = 38;
  camera->lookfrom = Point3(278, 278, -800);
  camera->lookat = Point3(278, 278, 0);
  camera->vup = Vec3(0, 1, 0);
  camera->defocus_angle = 0;
  camera->focus_dist = 10;
  camera_list.add_camera(camera);

  update_camera_list();
}

void MainWindow::init_scene()
{
  _scene->make_default_scene();
  update_objects_list();
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

  try
  {
    _render->set_camera(*camera_list.get_active_camera());
  }
  catch (const std::exception &e)
  {
    show_error("Ошибка", "Нет активной камеры");
    ui->renderButton->setEnabled(true);
    return;
  }

  _render->set_render_settings(final_render_settings);
  _world = _scene->get_objects();
  _futureWatcher.setFuture(QtConcurrent::run([this, size]()
                                             { _render->render(_world,
                                                               *_render_color_matrix,
                                                               cancel_running,
                                                               8,
                                                               [this]()
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
  QColor selectedColor = QColorDialog::getColor(Qt::white, this, "Выберите цвет");

  if (selectedColor.isValid())
  {
    QString style = QString("background-Color: rgb(%1, %2, %3);")
                        .arg(selectedColor.red())
                        .arg(selectedColor.green())
                        .arg(selectedColor.blue());
    ui->changeWorldColor->setStyleSheet(style);

    float r = selectedColor.redF();
    float g = selectedColor.greenF();
    float b = selectedColor.blueF();
    float a = selectedColor.alphaF();

    final_render_settings.background = Color{r, g, b};
    live_render_settings.background = Color{r, g, b};

    _livetime_render();
  }
}

void MainWindow::on_cameraDeleteButton_clicked()
{
  auto cams = get_selected(ui->cameraListWidget);
  if (cams.size() == 0)
  {
    show_error("Ошибка", "Нужно выбрать хотя бы одну камеру");
    return;
  }
  for (size_t id : cams)
  {
    camera_list.delete_camera(id);
  }
  update_camera_list();
}

void MainWindow::on_CameraAddDialogButton_clicked()
{
  CameraAddDialog dialog(this);
  Point3 vup = Point3{0, 1, 0};

  if (dialog.exec() == QDialog::Accepted)
  {
    try
    {
      auto camera = std::make_shared<Camera>(dialog.vfov(), dialog.lookfrom(), dialog.lookat(), vup);
      camera->defocus_angle = dialog.defocus_angle();
      camera->focus_dist = dialog.focus_dist();
      camera->id = camera_list.get_max_id() + 1;

      camera_list.add_camera(camera);
    }
    catch (const std::exception &e)
    {
      QMessageBox::critical(this, "Ошибка", "Невозможно добавить камеру.");
    }
    update_camera_list();
  }
}

void MainWindow::on_cameraEditButton_clicked()
{
  auto cams = get_selected(ui->cameraListWidget);
  if (cams.size() != 1)
  {
    show_error("Ошибка", "Для изменения необходимо выбрать ОДНУ камеру");
    return;
  }

  size_t id = cams[0];
  auto camera = camera_list.get_camera(id);

  CameraAddDialog dialog(
      camera->lookfrom, camera->lookat,
      camera->vfov, camera->defocus_angle,
      camera->focus_dist, this);
  Point3 vup = Point3{0, 1, 0};

  if (dialog.exec() == QDialog::Accepted)
  {
    try
    {
      camera->lookat = dialog.lookat();
      camera->lookfrom = dialog.lookfrom();
      camera->vfov = dialog.vfov();
      camera->defocus_angle = dialog.defocus_angle();
      camera->focus_dist = dialog.focus_dist();
    }
    catch (const std::exception &e)
    {
      QMessageBox::critical(this, "Ошибка", "Невозможно отредактировать камеру.");
    }
    _livetime_render();
  }
}

void MainWindow::on_cameraSetButton_clicked()
{
  auto cams = get_selected(ui->cameraListWidget);
  if (cams.size() != 1)
  {
    show_error("Ошибка", "Выбрать активной можно только одну камеру");
    return;
  }

  size_t id = cams[0];
  camera_list.set_active_camera(id);
  _livetime_render();
}

void MainWindow::on_objectDeletebutton_clicked()
{
  auto objects = get_selected(ui->objectListWidget);
  if (objects.size() == 0)
  {
    show_error("Ошибка", "Нужно выбрать хотя бы один объект");
    return;
  }
  for (size_t id : objects)
  {
    _scene->delete_object(id);
  }
  update_objects_list();
  _livetime_render();
}

void MainWindow::on_objectMakeEmitButton_clicked()
{
  auto objects = get_selected(ui->objectListWidget);
  if (objects.size() == 0)
  {
    show_error("Ошибка", "Нужно выбрать хотя бы один объект");
    return;
  }

  PowerColorDialog dialog(this);
  if (dialog.exec() == QDialog::Accepted)
  {
    double power = dialog.power();
    QColor color = dialog.color();

    Color c{color.redF(), color.greenF(), color.blueF()};
    qDebug() << "Мощность:" << power;
    qDebug() << "Цвет RGB:" << c.x() << c.y() << c.z();

    for (size_t id : objects)
    {
      _scene->add_light(id, power, c);
    }
  }

  _livetime_render();
}

void MainWindow::on_objectChangeMaterialButton_clicked()
{
  auto objects = get_selected(ui->objectListWidget);
  if (objects.size() == 0)
  {
    show_error("Ошибка", "Нужно выбрать хотя бы один объект");
    return;
  }

  MaterialDialog dialog(this);
  if (dialog.exec() != QDialog::Accepted)
    return;

  // Формируем MaterialStruct из данных диалога
  MaterialStruct mat;
  mat.type = dialog.selectedType();

  switch (mat.type)
  {
  case Lambertian_t:
  case Metal_t:
  {
    QColor qcolor = (mat.type == Lambertian_t)
                        ? dialog.lambertianColor()
                        : dialog.metalColor();
    mat.color = Color(qcolor.redF(), qcolor.greenF(), qcolor.blueF());
    break;
  }
  case Transparent_t:
    mat.refraction_index = dialog.refractionIndex();
    break;
  }

  if (mat.type == Metal_t)
  {
    mat.fuzz = dialog.fuzz();
  }

  try
  {

    for (size_t object_id : objects)
    {
      _scene->set_material(object_id, mat);
    }
    _livetime_render(); // обновляем отображение
  }
  catch (const std::exception &e)
  {
    QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
  }
}

void MainWindow::on_objectMoveButton_clicked()
{
  auto selected = get_selected(ui->objectListWidget);
  if (selected.empty())
  {
    QMessageBox::warning(this, "Ошибка", "Выберите объект для перемещения.");
    return;
  }

  MoveObjectDialog dialog(this);
  if (dialog.exec() != QDialog::Accepted)
    return;

  Point3 offset = dialog.offset();
  // size_t object_id = selected[0];

  try
  {
    for (size_t id : selected)
    {
      _scene->move_object(id, offset);
    }
    _livetime_render();
  }
  catch (const std::exception &e)
  {
    QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
  }
}

void MainWindow::on_objectRotateButton_clicked()
{
  auto selected = get_selected(ui->objectListWidget);
  if (selected.empty())
  {
    QMessageBox::warning(this, "Ошибка", "Выберите объект для поворота.");
    return;
  }

  RotateObjectDialog dialog(this);
  if (dialog.exec() != QDialog::Accepted)
    return;

  double ax = dialog.angleX();
  double ay = dialog.angleY();
  double az = dialog.angleZ();

  Vec3 rotate_info{ax, ay, az};
  try
  {
    for (size_t id : selected)
    {
      _scene->rotate_object(id, rotate_info);
    }
    _livetime_render();
  }
  catch (const std::exception &e)
  {
    QMessageBox::critical(this, "Ошибка", QString::fromStdString(e.what()));
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

  Camera camera;
  try
  {
    camera = *camera_list.get_active_camera();
  }
  catch (const std::exception &e)
  {
    show_error("Ошибка", "Нет активной камеры");
    ui->renderButton->setEnabled(true);
    return;
  }

  try
  {
    _world = _scene->get_objects();
  }
  catch (const std::exception &e)
  {
    show_error("Ошибка", "Ошибка при получении информации о мире");
    ui->renderButton->setEnabled(true);
    return;
  }

  StartSimpleRenderCommand command{live_render_settings, camera, _world, *_color_matrix, cancel_live_view_running};
  _facade->execute(command);

  _drawer->draw(*_color_matrix);

  set_scene();
}

void MainWindow::update_camera_list()
{
  ui->cameraListWidget->clear();
  for (size_t id : camera_list.get_camera_ids())
  {
    ui->cameraListWidget->addItem(QString::number(id));
  }
}

void MainWindow::update_objects_list()
{
  ui->objectListWidget->clear();
  for (size_t id : _scene->get_objects().get_objects_ids())
  {
    ui->objectListWidget->addItem(QString::number(id));
  }
}

std::vector<size_t> MainWindow::get_selected(QListWidget *label)
{
  std::vector<size_t> ids;
  for (int i = 0; i < label->count(); i++)
  {
    if (label->item(i)->isSelected())
    {
      ids.push_back(label->item(i)->text().toInt());
    }
  }
  return ids;
}

void MainWindow::show_error(const std::string label, const std::string message)
{
  QMessageBox::critical(
      this,
      QString::fromStdString(label),
      QString::fromStdString(message));
}

void MainWindow::on_objectAddButton_clicked()
{
  AddObjectDialog dialog(this);
  if (dialog.exec() != QDialog::Accepted)
    return;

  try
  {
    switch (dialog.selectedType())
    {
    case AddObjectDialog::Sphere:
      qDebug() << "add sphere";
      _scene->add_sphere(dialog.sphereCenter(), dialog.sphereRadius());
      break;

    case AddObjectDialog::Cylinder:
      qDebug() << "add cylender";
      _scene->add_cylinder(
          dialog.cylinderAxisCenter(),
          dialog.cylinderRadius(),
          dialog.cylinderY0(),
          dialog.cylinderY1());
      break;

    case AddObjectDialog::Cone:
      qDebug() << "add cone";
      _scene->add_cone(
          dialog.coneBaseCenter(),
          dialog.coneRadius(),
          dialog.coneHeight());
      break;

    case AddObjectDialog::QuadPyramid:
      qDebug() << "add quad pyramid";
      _scene->add_quad_pyramid(
          dialog.quadPyramidBaseCenter(),
          dialog.quadPyramidHalfWidth(),
          dialog.quadPyramidHeight());
      break;

    case AddObjectDialog::TriPyramid:
      qDebug() << "add triangle pyramid";
      _scene->add_triag_pyramid(
          dialog.triPyramidV0(),
          dialog.triPyramidV1(),
          dialog.triPyramidV2(),
          dialog.triPyramidApex());
      break;

    case AddObjectDialog::Box:
      qDebug() << "add box";
      _scene->add_box(dialog.boxPointA(), dialog.boxPointB());
      break;

    case AddObjectDialog::Quad:
      qDebug() << "add quad";
      _scene->add_quad(
          dialog.quadQ(),
          dialog.quadU(),
          dialog.quadV());
      break;

    case AddObjectDialog::Triangle:
      qDebug() << "add triag";
      _scene->add_triangle(
          dialog.triA(),
          dialog.triB(),
          dialog.triC());
      break;
    }

    _livetime_render();
    update_objects_list();
  }
  catch (const std::exception &e)
  {
    show_error("Ошибка", e.what());
  }
}

MainWindow::~MainWindow()
{
  delete ui;
}