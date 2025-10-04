#pragma once

#include "hittable.h"

class Material
{
public:
    virtual ~Material() = default;

    virtual bool scatter(
        const Ray &r_in, const HitRecord &rec, Color &attenuation, Ray &scattered) const
    {
        return false;
    }
};

class Lambertian : public Material
{
public:
    Lambertian(const Color &albedo) : _albedo(albedo) {}

    bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation, Ray &scattered) const override
    {
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

class Metal : public Material
{
public:
    Metal(const Color &albedo, double fuzz) : _albedo(albedo), _fuzz(fuzz < 1 ? fuzz : 1) {}

    bool scatter(const Ray &r_in, const HitRecord &rec, Color &attenuation, Ray &scattered) const override
    {
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