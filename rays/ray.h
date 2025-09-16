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

Color ray_color(Ray r)
{
    // Приведение к единичному вектору
    Vec3 unit_direction = unit_vector(r.direction());
    double a = 0.5 * (unit_direction.y() + 1.0);

    // По формуле линейной интерполяци
    // blended_value = (1 - a) * start_value + a * end_value
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}