#pragma once

#include "color_matrix.h"
#include "drawer.h"
#include "hittable_list.h"
#include "material.h"

#include <QSize>
#include <iostream>

class Scene
{
public:
  Scene()
  {
    _default_material = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
  }
  ~Scene() {}

  HittableList make_default_scene();
  HittableList &get_objects();

  void delete_object(size_t id);


  void add_sphere(const Point3 &center, double radius);
  

private:
  HittableList _objects;
  std::shared_ptr<Material> _default_material;
};