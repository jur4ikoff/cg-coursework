#pragma once

#include "constants.h"
#include "ray.h"

class Material;

class HitRecord {
public:
  Point3 point;
  Vec3 normal;
  shared_ptr<Material> material;
  double t;
  bool front_face;

  /**
   * @brief Устанавливает вектор нормали
   * @note Параметр outward_normal должен быть нормализован
   */
  void set_face_normal(const Ray &r, const Vec3 &outward_normal) {
    front_face = Vec3::dot(r.direction(), outward_normal) < 0;
    normal = front_face ? outward_normal : -outward_normal;
  }
};

class Hittable {
public:
  virtual ~Hittable() = default;

  virtual bool hit(const Ray &ray, Interval ray_t, HitRecord &rec) const = 0;
};