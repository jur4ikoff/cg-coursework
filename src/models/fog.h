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

class Smoke : public Hittable
{
public:
  // density — средняя плотность тумана (начните с 0.3–1.0)
  // scale — масштаб шума (меньше → крупнее облака; попробуйте 0.01–0.1)
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
    Point3 p = r.at(t);

    // Гарантированно ненулевая плотность
    double raw = noise.noise(scale * p);
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
  Perlin noise;
  double density; // основной параметр — начните с 0.5
  double scale;   // масштаб шума — начните с 0.02
  shared_ptr<Material> phase_function;
};

// class DynamicFog : public Hittable
// {
// public:
//   DynamicFog(shared_ptr<Hittable> boundary,
//              double max_density,       // верхняя оценка плотности
//              shared_ptr<Perlin> noise, // 3D шум Перлина
//              double noise_scale,       // масштаб шума в мире
//              shared_ptr<Texture> tex)
//       : boundary(boundary),
//         max_density(max_density),
//         neg_inv_max_density(-1.0 / max_density),
//         noise(noise),
//         noise_scale(noise_scale),
//         phase_function(make_shared<isotropic>(tex))
//   {
//   }

//   DynamicFog(shared_ptr<Hittable> boundary,
//              double max_density,
//              shared_ptr<Perlin> noise,
//              double noise_scale,
//              const Color &albedo)
//       : boundary(boundary),
//         max_density(max_density),
//         neg_inv_max_density(-1.0 / max_density),
//         noise(noise),
//         noise_scale(noise_scale),
//         phase_function(make_shared<isotropic>(albedo))
//   {
//   }

//   bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
//   {
//     HitRecord rec1, rec2;

//     // Ищем вход и выход луча из граничного объекта
//     if (!boundary->hit(r, Interval::universe, rec1))
//       return false;

//     if (!boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
//       return false;

//     if (rec1.t < ray_t.min)
//       rec1.t = ray_t.min;
//     if (rec2.t > ray_t.max)
//       rec2.t = ray_t.max;
//     if (rec1.t >= rec2.t)
//       return false;
//     if (rec1.t < 0)
//       rec1.t = 0;

//     const auto ray_length = r.direction().length();
//     auto t = rec1.t;

//     // Woodcock / delta tracking по отрезку [rec1.t, rec2.t]
//     while (true)
//     {
//       // Сэмплируем расстояние до кандидата столкновения
//       double hit_distance = neg_inv_max_density * std::log(random_double());
//       t += hit_distance / ray_length;

//       if (t >= rec2.t)
//         return false; // вышли из среды — столкновения нет

//       Point3 p = r.at(t);

//       // Локальная плотность из шума Перлина в [0, max_density]
//       double local_density = density(p);

//       // Принимаем/отбрасываем кандидата
//       double accept_prob = local_density / max_density;
//       if (random_double() < accept_prob)
//       {
//         rec.t = t;
//         rec.p = p;
//         rec.normal = Vec3(1, 0, 0); // произвольная нормаль
//         rec.front_face = true;
//         rec.mat = phase_function;
//         return true;
//       }
//     }
//   }

//   Aaab bounding_box() const override
//   {
//     return boundary->bounding_box();
//   }

// private:
//   // Плотность в точке по 3D‑шуму Перлина
//   double density(const Point3 &p) const
//   {
//     // Предполагаем, что noise->turb(...) даёт значения ~[-1, 1]
//     double n = noise->turb(p * noise_scale, 7);
//     // Приводим к [0, 1]
//     n = 0.5 * (n + 1.0);

//     // Можно добавить минимальную плотность, чтобы туман нигде не обнулялся:
//     double min_factor = 0.2; // 0 = может быть полностью прозрачно, 1 = везде max_density
//     double factor = min_factor + (1.0 - min_factor) * n;

//     return max_density * factor;
//   }

//   // double density(const Point3 &p) const
//   // {
//   //   // 1. Берем турбулентность (дает значения от 0 до ~1 с фрактальной детализацией)
//   //   // 7 — глубина рекурсии (octaves), как в книге
//   //   double noise_val = noise->turb(p * noise_scale, 7);

//   //   // 2. (Опционально) Сдвиг и обрезка, чтобы сделать промежутки чистыми
//   //   // Если шум < 0.3, считаем, что там пусто. Это разделит клубы.
//   //   double threshold = 0.3;
//   //   if (noise_val < threshold)
//   //     return 0.0;

//   //   // Нормализуем обратно в 0..1 после обрезки
//   //   noise_val = (noise_val - threshold) / (1.0 - threshold);

//   //   // 3. Возводим в степень для жесткости краев ("контраст")
//   //   // Чем больше степень, тем четче границы клубов.
//   //   // Попробуйте значения от 2.0 до 4.0
//   //   noise_val = std::pow(noise_val, 2.0); // "Квадратичная" плотность

//   //   // 4. Умножаем на максимальную плотность среды
//   //   return max_density * noise_val;
//   // }

//   shared_ptr<Hittable> boundary;
//   double max_density;
//   double neg_inv_max_density;
//   shared_ptr<Perlin> noise;
//   double noise_scale;
//   shared_ptr<Material> phase_function;
// };

class DynamicFog : public Hittable
{
public:
  DynamicFog(shared_ptr<Hittable> boundary, double max_density,
             shared_ptr<Perlin> noise, double scale, shared_ptr<Texture> tex)
      : boundary(boundary),
        max_density(max_density),
        inv_max_density(1.0 / max_density), // Используем положительное значение для удобства
        noise(noise),
        scale(scale),
        phase_function(make_shared<isotropic>(tex))
  {
  }

  DynamicFog(shared_ptr<Hittable> boundary, double max_density,
             shared_ptr<Perlin> noise, double scale, const Color &albedo)
      : boundary(boundary),
        max_density(max_density),
        inv_max_density(1.0 / max_density),
        noise(noise),
        scale(scale),
        phase_function(make_shared<isotropic>(albedo)) {}

  bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
  {
    HitRecord rec1, rec2;

    // 1. Находим границы входа и выхода из объема
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

    const auto ray_length = r.direction().length();
    double current_t = rec1.t;

    // ЗАЩИТА ОТ ЗАВИСАНИЯ: Ограничиваем число шагов
    int safety_counter = 0;
    const int MAX_STEPS = 10000;

    while (safety_counter++ < MAX_STEPS)
    {
      // 2. Делаем случайный шаг вперед.
      // Чем больше max_density, тем короче шаги.
      // log(random) отрицательный, поэтому ставим минус перед ним.
      double step = -std::log(random_double()) * inv_max_density;

      current_t += step / ray_length;

      // 3. Если вышли за границу объекта — попадания не было
      if (current_t >= rec2.t)
        return false;

      Point3 p = r.at(current_t);

      // 4. Вычисляем реальную плотность в этой точке
      double d = get_density(p);

      // 5. Проверка вероятности: принимаем ли мы это столкновение?
      // d / max_density — вероятность от 0 до 1
      if (random_double() < (d * inv_max_density))
      {
        rec.t = current_t;
        rec.p = p;
        rec.normal = Vec3(1, 0, 0); // Для объема нормаль не важна
        rec.front_face = true;
        rec.mat = phase_function;
        return true;
      }
    }

    // Если превысили лимит шагов, считаем, что пролетели насквозь (оптимизация)
    return false;
  }

  Aaab bounding_box() const override { return boundary->bounding_box(); }

private:
  // Простая и надежная функция плотности для клубов
  double get_density(const Point3 &p) const
  {
    // turb возвращает значение >= 0. Обычно 0..1
    double noise_val = noise->turb(p * scale, 7);

    // Усиливаем контраст (клубы):
    // Возведение в квадрат делает слабые значения очень маленькими -> прозрачность
    noise_val = noise_val * noise_val;

    // Принудительно ограничиваем, чтобы не превысить max_density
    if (noise_val > 1.0)
      noise_val = 1.0;

    return max_density * noise_val;
  }

  shared_ptr<Hittable> boundary;
  double max_density;
  double inv_max_density; // Храним 1/max_density
  shared_ptr<Perlin> noise;
  double scale;
  shared_ptr<Material> phase_function;
};
