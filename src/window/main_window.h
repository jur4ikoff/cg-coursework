#pragma once

#include "color_matrix.h"
#include "drawer.h"
#include "qt_drawer.h"

#include "scene.h"
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

namespace Ui {
class MainWindow;
}

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
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

private:
  volatile bool cancel_running;

  std::shared_ptr<Drawer> _drawer;
  std::shared_ptr<Scene> _scene;
  std::shared_ptr<QGraphicsScene> _qt_scene;
  std::shared_ptr<QPixmap> _pixmap;
  std::shared_ptr<ColorMatrix> _color_matrix;
  std::shared_ptr<ColorMatrix> _render_color_matrix;

  QtRenderManager *_render_manager;

  QFutureWatcher<void> _futureWatcher;

  // Для окна рендера
  QWidget *_popup = nullptr;
  QLabel *_render_label = nullptr;

  std::shared_ptr<QPixmap> _render_pixmap;

  Ui::MainWindow *ui;

  // Обновление сцен
  void set_scene();
  void update_render_scene();

  //
  void disable_render_button();
};
