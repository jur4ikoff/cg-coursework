#pragma once

#include "color_matrix.h"
#include "drawer.h"
#include "hittable_list.h"

#include <QSize>
#include <iostream>

class Scene
{
public:
  Scene() {}
  hittable_list draw();
  ~Scene() {}
};