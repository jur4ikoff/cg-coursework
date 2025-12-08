#pragma once

#include "color_matrix.h"
#include "drawer.h"
#include "hittable_list.h"
#include "material.h"
#include "material_struct.h"

#include <QSize>
#include <iostream>

class Scene
{
public:
  Scene();
  ~Scene() {}

  HittableList make_default_scene();
  HittableList &get_objects();

  void set_material(size_t id, MaterialStruct &mat_struct);
  void move_object(size_t id, Vec3 offset);

  void add_light(size_t id, double power, Color &color);
  void delete_object(size_t id);

  void add_sphere(const Point3 &center, double radius);
  void add_cylinder(const Point3 &center, double radius, double ymin, double ymax);
  void add_cone(const Point3 &center, double radius, double height);
  void add_quad_pyramid(const Point3 &center, double half_width, double height);
  void add_triag_pyramid(const Point3 &v0, const Point3 &v1, const Point3 &v2, const Point3 &up);
  void add_box(const Point3 &point1, const Point3 &point2);
  void add_quad(const Point3 &left_down, const Vec3 &u, const Vec3 &v);
  void add_triangle(const Point3 &v1, const Point3 &v2, const Point3 &v3);

private:
  HittableList _objects;
  std::shared_ptr<Material> _default_material;
};