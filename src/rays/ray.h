#pragma once

#include "color.h"
#include "vec3.h"

class Ray {
public:
  Ray() {}

  Ray(const Point3 &origin, const Vec3 &direction)
      : _origin(origin), _direction(direction) {}

  const Point3 &origin() const { return _origin; }
  const Vec3 &direction() const { return _direction; }

  Point3 at(double t) const { return _origin + t * _direction; }

private:
  Point3 _origin;
  Vec3 _direction;
};
