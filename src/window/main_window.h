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

#define MIN_SCENE_WIDTH 600
#define MIN_SCENE_HEIGHT 600

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
  std::shared_ptr<Render> _render;

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

  Camera camera;


  std::shared_ptr<QPixmap> _render_pixmap;

  // Обновление сцен
  void set_scene();
  void update_render_scene();
};
