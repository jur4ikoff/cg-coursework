#pragma once

#include "color_matrix.h"
#include "drawer.h"

#include <QSize>
#include <iostream>

class Scene {
public:
  Scene() {}

  void draw(QSize size, ColorMatrix &color_matrix,
            volatile bool &cancel_request,
            std::function<void()> tile_callback = nullptr);

  void measure_1(ColorMatrix &color_matrix);
  void measure_2(ColorMatrix &color_matrix);
  ~Scene() {}
};
