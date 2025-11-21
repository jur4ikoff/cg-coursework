#pragma once

#include "color_matrix.h"

#include <QObject>
#include <QPixmap>
#include <iostream>

class QtDrawer {
public:
  QtDrawer(QPixmap &pixmap) : _pixmap(pixmap) {}

  void draw(const ColorMatrix &matrix) {

    QImage image = matrix.to_QImage();
    _pixmap = QPixmap::fromImage(image);
  }

  ~QtDrawer() {}

private:
  QPixmap &_pixmap;
};
