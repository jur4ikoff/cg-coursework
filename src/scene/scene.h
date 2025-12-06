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
  HittableList draw();
  void make_default_scene();
  HittableList &get_objects();
  ~Scene() {}
private:
  HittableList _objects;
};