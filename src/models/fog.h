#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "material.h"
#include "texture.h"

class ConstantFog : public Hittable
{
public:
  ConstantFog(shared_ptr<Hittable> boundary, double density, shared_ptr<Texture> tex)
      : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(make_shared<isotropic>(tex))
  {
  }

  ConstantFog(shared_ptr<Hittable> boundary, double density, const color &albedo)
      : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(make_shared<isotropic>(albedo))
  {
  }

  bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
  {
    HitRecord rec1, rec2;

    if (!boundary->hit(r, Interval::universe, rec1))
      return false;

    if (!boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
      return false;

    if (rec1.t < ray_t.min)
      rec1.t = ray_t.min;
    if (rec2.t > ray_t.max)
      rec2.t = ray_t.max;

    if (rec1.t >= rec2.t)
      return false;

    if (rec1.t < 0)
      rec1.t = 0;

    auto ray_length = r.direction().length();
    auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    auto hit_distance = neg_inv_density * std::log(random_double());

    if (hit_distance > distance_inside_boundary)
      return false;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    rec.normal = Vec3(1, 0, 0); // arbitrary
    rec.front_face = true;      // also arbitrary
    rec.mat = phase_function;

    return true;
  }

  Aaab bounding_box() const override { return boundary->bounding_box(); }

private:
  shared_ptr<Hittable> boundary;
  double neg_inv_density;
  shared_ptr<Material> phase_function;
};

class NonuniformFog : public Hittable
{
public:
  // density — средняя плотность тумана (начните с 0.3–1.0)
  // scale — масштаб шума (меньше → крупнее облака; попробуйте 0.01–0.1)
  NonuniformFog(
      shared_ptr<Hittable> boundary,
      shared_ptr<Perlin> noise,
      double density,
      double scale,
      shared_ptr<Material> phase) : boundary(boundary), noise(noise), density(density), scale(scale), phase_function(phase) {}

  NonuniformFog(
      shared_ptr<Hittable> boundary,
      shared_ptr<Perlin> noise,
      double density,
      double scale,
      const color &albedo) : NonuniformFog(boundary, noise, density, scale, make_shared<isotropic>(albedo)) {}

  bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
  {
    HitRecord rec1, rec2;
    if (!boundary->hit(r, Interval::universe, rec1))
      return false;
    if (!boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
      return false;

    auto t0 = std::fmax(rec1.t, ray_t.min);
    auto t1 = std::fmin(rec2.t, ray_t.max);
    if (t0 >= t1)
      return false;

    auto ray_len = r.direction().length();
    auto dist = (t1 - t0) * ray_len;

    // Увеличьте "базовую" плотность для надёжности
    const double base_density = 1.0; // фиксированная максимальная плотность
    auto hit_dist = -std::log(random_double()) / base_density;
    if (hit_dist > dist)
      return false;

    auto t = t0 + hit_dist / ray_len;
    point3 p = r.at(t);

    // 🔥 Гарантированно ненулевая плотность
    double raw = noise->noise(scale * p);
    double noise_val = 0.2 + 0.8 * std::fabs(raw); // [0.2, 1.0]
    double real_density = density * noise_val;

    // Принимаем, если real_density > случайного порога
    // if (random_double() > real_density)
    if (real_density < 1e-5 || random_double() > std::min(real_density, 1.0))
      return false;

    rec.t = t;
    rec.p = p;
    rec.normal = Vec3(1, 0, 0);
    rec.front_face = true;
    rec.mat = phase_function;
    return true;
  }

  Aaab bounding_box() const override
  {
    return boundary->bounding_box();
  }

private:
  shared_ptr<Hittable> boundary;
  shared_ptr<Perlin> noise;
  double density; // основной параметр — начните с 0.5
  double scale;   // масштаб шума — начните с 0.02
  shared_ptr<Material> phase_function;
};

#endif