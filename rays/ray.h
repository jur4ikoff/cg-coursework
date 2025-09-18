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

bool hit_sphere(const Point3 &center, double radius, const Ray &r)
{
    // Решаем квадраное уравнение t^2 * d - 2td * (C - Q) + (C - Q) * (C - Q) - r^2 =0
    // Вектор между началом луча и центром окруност
    Vec3 oc = center - r.origin();

    // Скалряное проивзедения направления вектора на самого себя
    double a = dot(r.direction(), r.direction());
    double b = -2.0 * dot(r.direction(), oc);
    double c = dot(oc, oc) - radius * radius;

    double discriminant = b * b - 4 * a * c;
    return (discriminant >= 0);
}

Color ray_color(Ray r)
{
    if (hit_sphere(Point3(0, 0, 1), 0.5, r))
    {
        return Color(1, 0, 0);
    }

    return sky(r);
}
