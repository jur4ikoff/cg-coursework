#pragma once

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

  ConstantFog(shared_ptr<Hittable> boundary, double density, const Color &albedo)
      : boundary(boundary), neg_inv_density(-1 / density),
        phase_function(make_shared<isotropic>(albedo))
  {
  }

  bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
  {
    HitRecord rec1, rec2;

    if (!is_visible)
      return false;

    // нахождения вход и выхода из тумана
    if (!boundary->hit(r, Interval::universe, rec1))
      return false;
    if (!boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
      return false;

    // Ограничиваем отрезок тумана интервалом
    if (rec1.t < ray_t.min)
      rec1.t = ray_t.min;
    if (rec2.t > ray_t.max)
      rec2.t = ray_t.max;
    if (rec1.t >= rec2.t)
      return false;
    if (rec1.t < 0)
      rec1.t = 0;

    // длина луча
    auto ray_length = r.direction().length();
    // дистанция которая пройдена внутри тумана
    auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    // Расстояние до точки рассеяния:
    auto hit_distance = neg_inv_density * std::log(random_double());

    if (hit_distance > distance_inside_boundary)
      return false;

    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    rec.normal = Vec3(1, 0, 0); // произвольная
    rec.front_face = true;      // произвольно
    rec.mat = phase_function;

    return true;
  }

  Aaab bounding_box() const override { return boundary->bounding_box(); }

  double get_density()
  {
    return -1 / neg_inv_density;
  }

  void set_density(double density)
  {
    neg_inv_density = -1 / density;
  }

private:
  shared_ptr<Hittable> boundary;
  double neg_inv_density;
  shared_ptr<Material> phase_function;
};

class Smoke : public Hittable
{
public:
  // density — средняя плотность тумана (начните с 0.3–1.0)
  // scale — масштаб шума (меньше → крупнее облака; попробуйте 0.001–0.1)
  Smoke(
      shared_ptr<Hittable> boundary,
      double density,
      double scale,
      shared_ptr<Material> phase) : boundary(boundary), density(density), scale(scale), phase_function(phase) {}

  Smoke(
      shared_ptr<Hittable> boundary,
      double density,
      double scale,
      const Color &albedo) : Smoke(boundary, density, scale, make_shared<isotropic>(albedo)) {}

  bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
  {
    HitRecord rec1, rec2;

    if (!is_visible)
      return false;
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

    auto hit_dist = -std::log(random_double()) / density;
    if (hit_dist > dist)
      return false;

    auto t = t0 + hit_dist / ray_len;
    Point3 p = r.at(t);

    // Гарантированно ненулевая плотность
    double raw = noise.noise(scale * p);
    double noise_val = 0.1 + 0.9 * std::fabs(raw); // [0.1, 1.0]
    // double noise_val = std::fabs(raw); // [0.2, 1.0]
    double real_density = density * noise_val;

    // Принимаем, если real_density > случайного порога
    // if (real_density < 1e-5 || random_double() > std::min(real_density, 1.0))
    // return false;

    if (real_density < 1e-6 || random_double() > real_density)
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

  double get_density()
  {
    return density;
  }

  void set_density(double _density)
  {
    density = _density;
  }

  double get_scale()
  {
    return scale;
  }

  void set_scale(double _scale)
  {
    scale = _scale;
  }

private:
  shared_ptr<Hittable> boundary;
  Perlin noise;
  double density; // основной параметр — начните с 0.5
  double scale;   // масштаб шума — начните с 0.02
  shared_ptr<Material> phase_function;
};

class GroundSmoke : public Hittable
{
public:
  GroundSmoke(
      shared_ptr<Hittable> boundary,
      double density,
      double scale,
      double height_falloff, // контролирует, насколько быстро дым исчезает с высотой
      shared_ptr<Material> phase)
      : boundary(boundary), density(density), scale(scale),
        height_falloff(height_falloff), phase_function(phase)
  {
  }

  GroundSmoke(
      shared_ptr<Hittable> boundary,
      double density,
      double scale,
      double height_falloff,
      const Color &albedo)
      : GroundSmoke(boundary, density, scale, height_falloff, make_shared<isotropic>(albedo)) {}

  bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
  {
    HitRecord rec1, rec2;
    if (!is_visible)
      return false;
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

    // Базовая модель экспоненциального затухания
    // const double base_density = 1.0;
    auto hit_dist = -std::log(random_double()) / density;
    if (hit_dist > dist)
      return false;

    // Находим точку на луче
    auto t = t0 + hit_dist / ray_len;
    Point3 p = r.at(t);

    // Текстура с использованием шума Перлина
    double raw_noise = noise.noise(scale * p);
    double noise_val = 0.2 + 0.8 * std::fabs(raw_noise); // [0.2, 1.0]

    // Множитель по высоте: чем ниже y — тем больше плотность
    // Ограничиваем y снизу, чтобы избежать экспоненциального роста при y → -inf
    double y = p.y();
    double height_factor = std::exp(-height_falloff * std::max(y, -10.0));

    // Итоговая плотность
    double real_density = density * noise_val * height_factor;

    // Отсев: если плотность слишком мала или случайный порог не пройден — нет попадания
    // Вы уже выбрали точку, предполагая худший (самый густой) случай.
    // Но если в этой точке туман редкий, то рассеяние менее вероятно.
    // Поэтому вы "отклоняете" эту точку с вероятностью 1 - real_density.
    if (real_density < 1e-5 || random_double() > std::min(real_density, 1.0))
      return false;

    rec.t = t;
    rec.p = p;
    rec.normal = Vec3(0, 1, 0); // нормаль вверх (по Y), можно и оставить (1,0,0), но логичнее по высоте
    rec.front_face = true;
    rec.mat = phase_function;
    return true;
  }

  Aaab bounding_box() const override
  {
    return boundary->bounding_box();
  }

  double get_density()
  {
    return density;
  }

  void set_density(double _density)
  {
    density = _density;
  }

  double get_scale()
  {
    return scale;
  }

  void set_scale(double _scale)
  {
    scale = _scale;
  }

  double get_height_falloff()
  {
    return height_falloff;
  }

  void set_height_falloff(double falloff)
  {
    height_falloff = falloff;
  }

private:
  shared_ptr<Hittable> boundary;
  Perlin noise;
  double density;        // базовая плотность (например, 0.5–2.0)
  double scale;          // масштаб шума (0.002)
  double height_falloff; // 0-0.003
  shared_ptr<Material> phase_function;
};