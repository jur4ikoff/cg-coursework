#pragma once

#include <memory>

class QPixmap;
class QtDrawer;
class QtRenderManager;
class ColorMatrix;

class Drawer {
public:
  explicit Drawer(QPixmap &pixmap);
  ~Drawer();
  void draw(const ColorMatrix &color_matrix);

private:
  std::unique_ptr<QtDrawer> _impl;
};