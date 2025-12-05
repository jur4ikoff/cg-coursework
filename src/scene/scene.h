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
  HittableList leave_only_seample_objects(HittableList &objects);
  ~Scene() {}
private:
  HittableList _objects;
};