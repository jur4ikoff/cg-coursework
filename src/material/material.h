#pragma once

#include "hittable.h"

class Material {
public:
  virtual ~Material() = default;

  virtual bool scatter(const Ray &r_in, const HitRecord &rec,
                       Color &attenuation, Ray &scattered) const {
    return false;
  }
};

class Lambertian : public Material {
public:
  Lambertian(const Color &albedo) : _albedo(albedo) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    Vec3 scatter_dirrection = rec.normal + Vec3::random_unit_vector();

    if (scatter_dirrection.near_zero())
      scatter_dirrection = rec.normal;

    scattered = Ray(rec.point, scatter_dirrection);
    attenuation = _albedo;
    return true;
  }

private:
  Color _albedo;
};

class Metal : public Material {
public:
  Metal(const Color &albedo, double fuzz)
      : _albedo(albedo), _fuzz(fuzz < 1 ? fuzz : 1) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    Vec3 reflected = reflect(r_in.direction(), rec.normal);
    reflected = unit_vector(reflected) + (_fuzz * Vec3::random_unit_vector());
    scattered = Ray(rec.point, reflected);
    attenuation = _albedo;
    return (Vec3::dot(scattered.direction(), rec.normal) > 0);
  }

private:
  Color _albedo;
  double _fuzz;
};

class Dielectric : public Material {
public:
  Dielectric(double refraction_index) : _reftaction_index(refraction_index) {}

  bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation,
               Ray &scattered) const override {
    // attenuation = Color(1.0, 1.0, 1.0);
    // double ri = rec.front_face ? (1.0 / _reftaction_index) :
    // _reftaction_index;

    // Vec3 unit_direction = unit_vector(r_in.direction());
    // Vec3 refracted = refract(unit_direction, rec.normal, ri);

    // scattered = Ray(rec.point, refracted);
    // return true;
    attenuation = Color(1.0, 1.0, 1.0);
    double ri = rec.front_face ? (1.0 / _reftaction_index) : _reftaction_index;

    Vec3 unit_direction = unit_vector(r_in.direction());
    double cos_theta = std::fmin(Vec3::dot(-unit_direction, rec.normal), 1.0);
    double sin_theta = std::sqrt(1.0 - cos_theta * cos_theta);

    bool cannot_refract = ri * sin_theta > 1.0;
    Vec3 direction;

    if (cannot_refract || reflectance(cos_theta, ri) > random_double()) {
      direction = reflect(unit_direction, rec.normal);
    } else {
      direction = refract(unit_direction, rec.normal, ri);
    }

    scattered = Ray(rec.point, direction);
    return true;
  }

private:
  double _reftaction_index;

  double reflectance(double cosine, double refraction_index) const {
    auto r0 = (1 - refraction_index) / (1 + refraction_index);
    r0 *= r0;
    return r0 + (1 - r0) * std::pow((1 - cosine), 5);
  }
};