#pragma once

#include "hittable.h"
#include "vec3.h"


class Sphere : public Hittable
{
public:
    Sphere(const Point3 &center, double radius) : _center(center), _radius(std::fmax(0, radius)) {}

    bool hit(const Ray &ray, double ray_tmin, double ray_tmax, HitRecord &rec) const override
    {
        Vec3 oc = _center - ray.origin();
        double a = ray.direction().lenght_squared();
        double h = dot(ray.direction(), oc);
        double c = oc.lenght_squared() - _radius * _radius;

        double discriminant = h * h - a * c;
        if (discriminant < 0){
            return false;
        }

        double sqrtd = std::sqrt(discriminant);

        double root = (h - sqrtd) / a;
        if (root <= ray_tmin || ray_tmax <= root)
        {
            return false;
        }

        rec.t = root;
        rec.point = ray.at(rec.t);
        Vec3 outward_normal = (rec.point - _center) / _radius;
        rec.set_face_normal(ray, outward_normal);

        return true;
    }

private:
    Point3 _center;
    double _radius;
};