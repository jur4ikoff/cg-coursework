#pragma once

#include "hittable.h"
#include "texture.h"

class Material {
public:
  virtual ~Material() = default;

  // Светимость
  virtual Color emitted(const Point3 &p) const { return Color(0, 0, 0); }

  // Разброс
  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       Color &attenuation, Ray &scattered) const {
    return false;
  }

  size_t id;
};

class Lambertian : public Material {
public:
  Lambertian(const Color &albedo) : tex(make_shared<SolidColor>(albedo)) {}

  Lambertian(shared_ptr<Texture> tex) : tex(tex) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    // auto scatter_direction = rec.normal + random_unit_vector();
    // auto scatter_direction = random_on_hemisphere(rec.normal);
    // auto scatter_direction =
    // random_cosine_direction_in_hemisphere(rec.normal);
    auto scatter_direction = cosine_sample_hemisphere(rec.normal);

    // Ловим близость к нулю направления разброса
    if (scatter_direction.near_zero())
      scatter_direction = rec.normal;

    scattered = Ray(rec.p, scatter_direction);
    attenuation = tex->value(rec.p);
    return true;
  }

private:
  shared_ptr<Texture> tex;
};

class Metal : public Material {
public:
  Metal(const Color &albedo, double fuzz)
      : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    Vec3 reflected = reflect(r_in.direction(), rec.normal);
    reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
    scattered = Ray(rec.p, reflected);
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0);
  }

private:
  Color albedo;
  double fuzz;
};

class Transparent : public Material {
public:
  Transparent(double refraction_index) : refraction_index(refraction_index) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    attenuation = Color(1.0, 1.0, 1.0);
    double ri = rec.front_face ? (1.0 / refraction_index) : refraction_index;

    Vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = std::fmin(dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    /*
    Если ri * sin_theta > 1, то по закону Снеллиуса преломление невозможно (это
    условие полного внутреннего отражения). В этом случае луч всегда отражается.
    */
    bool cannot_refract = ri * sin_theta > 1.0;
    Vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ri) > random_double())
      direction = reflect(unit_direction, rec.normal);
    else
      direction = refract(unit_direction, rec.normal, ri);

    scattered = Ray(rec.p, direction);
    return true;
  }

private:
  double refraction_index;

  static double reflectance(double cosine, double refraction_index) {
    // Use Schlick's approximation for reflectance.
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 = r0 * r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }
};

class diffuse_light : public Material {
public:
  diffuse_light(const Color &emit_light)
      : tex(make_shared<SolidColor>(emit_light)) {}

  diffuse_light(shared_ptr<Texture> tex) : tex(tex) {}

  Color emitted(const Point3 &p) const override { return tex->value(p); }

private:
  shared_ptr<Texture> tex;
};

class isotropic : public Material {
public:
  isotropic(const Color &albedo) : tex(make_shared<SolidColor>(albedo)) {}

  isotropic(shared_ptr<Texture> tex) : tex(tex) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    scattered = Ray(rec.p, random_unit_vector());
    attenuation = tex->value(rec.p);
    return true;
  }

private:
  shared_ptr<Texture> tex;
};
