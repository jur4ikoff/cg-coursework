#pragma once

#include "vec3.h"
#include "color.h"

class Ray
{
public:
    Ray() {}

    Ray(const MyPoint3 &origin, const MyVec3 &direction) : _origin(origin), _direction(direction) {}

    const MyPoint3 &origin() const { return _origin; }
    const MyVec3 &direction() const { return _direction; }

    MyPoint3 at(double t) const
    {
        return _origin + t * _direction;
    }

private:
    MyPoint3 _origin;
    MyVec3 _direction;
};
