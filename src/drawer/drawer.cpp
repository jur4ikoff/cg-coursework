#include "drawer.h"
#include "qt_drawer.h"

#include <Qpixmap>
#include <iostream>

Drawer::~Drawer() = default; // ← или просто {}

Drawer::Drawer(QPixmap &pixmap) { _impl = std::make_unique<QtDrawer>(pixmap); }

void Drawer::draw(const ColorMatrix &color_matrix) {
  _impl->draw(color_matrix);
}