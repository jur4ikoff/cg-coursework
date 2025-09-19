#pragma once

#include "ray.h"

class hit_record
{
public:
    Point3 _point;
    Vec3 _normal;
    double _t;
};

class Hitable
{
public:
    virtual ~Hittable() = default;

    Virtual bool hit(const Ray &ray, double ray_tmin, double ray_tmax, hit_record &rec);
};