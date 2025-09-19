#pragma once

#include "vec3.h"
#include "color.h"

class Ray
{
public:
    Ray() {}

    Ray(const Point3 &origin, const Vec3 &direction) : _origin(origin), _direction(direction) {}

    const Point3 &origin() const { return _origin; }
    const Vec3 &direction() const { return _direction; }

    Point3 at(double t) const
    {
        return _origin + t * _direction;
    }

private:
    Point3 _origin;
    Vec3 _direction;
};

Color sky(Ray r)
{
    // Приведение к единичному вектору
    Vec3 unit_direction = unit_vector(r.direction());
    double a = 0.5 * (unit_direction.y() + 1.0);

    // По формуле линейной интерполяци
    // blended_value = (1 - a) * start_value + a * end_value
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

double hit_sphere(const Point3 &center, double radius, const Ray &r)
{
    // Решаем квадраное уравнение t^2 * d - 2td * (C - Q) + (C - Q) * (C - Q) - r^2 =0
    // Вектор между началом луча и центром окруност
    Vec3 oc = center - r.origin();

    double a = r.direction().lenght_squared();
    double h = dot(r.direction(), oc);
    double c = oc.lenght_squared() - radius * radius;

    double discriminant = h * h - a * c;

    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (h - std::sqrt(discriminant)) / a;
    }
}

Color ray_color(Ray r)
{
    Point3 center(0, 0, -1);
    double t = hit_sphere(center, 0.5, r);
    if (t > 0.0)
    {
        Vec3 N = unit_vector(r.at(t) - center);
        return 0.5 * Color(N.x() + 1, N.y() + 1, N.z() + 1);
    }

    return sky(r);
}
