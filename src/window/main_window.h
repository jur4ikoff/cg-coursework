#pragma once

#include "color_matrix.h"
#include "drawer.h"
#include "qt_drawer.h"

#include "facade.h"
#include "scene.h"
#include "render.h"

#include "ui_main_window.h"
#include <QApplication>
#include <QFutureWatcher>
#include <QGraphicsPixmapItem>
#include <QGraphicsScene>
#include <QLabel>
#include <QMainWindow>
#include <QPixmap>
#include <QWidget>

#define DEFAULT_WIDTH 1200
#define DEFAULT_HEIGHT 800

#define MIN_SCENE_WIDTH 580
#define MIN_SCENE_HEIGHT 580

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
  ~MainWindow();

protected:
  void resizeEvent(QResizeEvent *event) override;

private slots:
  // Кнопки
  void on_renderButton_clicked();
  void on_changeWorldColor_clicked();

  void on_CameraAddDialogButton_clicked();
  void on_cameraDeleteButton_clicked();
  void on_cameraEditButton_clicked();
  void on_cameraSetButton_clicked();

  void on_objectAddButton_clicked();
  void on_objectDeletebutton_clicked();
  void on_objectMakeEmitButton_clicked();
  void on_objectChangeMaterialButton_clicked();
  void on_objectMoveButton_clicked();
  void on_objectRotateButton_clicked();
  void on_objectChangeVisibilityButton_clicked();

  void on_FogAddDialogButton_clicked();

  void tile_render_finished_slot();
  void pop_up_closed_slot();

protected:
  void closeEvent(QCloseEvent *event) override;

private:
  Ui::MainWindow *ui;

  volatile bool cancel_running;
  volatile bool cancel_live_view_running;
  bool is_mw_closed = false;

  std::shared_ptr<Facade> _facade;
  std::shared_ptr<Drawer> _drawer;
  std::shared_ptr<Drawer> _render_drawer;
  std::shared_ptr<Scene> _scene;

  // Настройка рендера
  std::shared_ptr<Render> _render;
  RenderSettings final_render_settings;
  RenderSettings live_render_settings{1, 3, Color{0.5, 0.5, 0.5}};

  std::shared_ptr<QGraphicsScene> _qt_scene;
  std::shared_ptr<QPixmap> _pixmap;
  std::shared_ptr<ColorMatrix> _color_matrix;
  std::shared_ptr<ColorMatrix> _render_color_matrix;

  HittableList _world;

  QtRenderManager *_render_manager;
  QFutureWatcher<void> _futureWatcher;

  // Для окна рендера
  QWidget *_popup = nullptr;
  QLabel *_render_label = nullptr;

  CameraList camera_list;

  std::shared_ptr<QPixmap> _render_pixmap;

  // Обновление сцен
  void set_scene();
  void update_render_scene();

  // Рендер в окне программы
  void _livetime_render();

  // Иниты
  void init_camera();
  void init_scene();

  // Получить выделенные
  std::vector<size_t> get_selected(QListWidget *label);

  // Списки
  void update_camera_list();
  void update_objects_list();

  // Ошибки
  void show_error(const std::string label, const std::string message);
};
