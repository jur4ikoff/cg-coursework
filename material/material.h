#pragma once

#include "hittable.h"

class Material
{
public:
    virtual ~Material() = default;

    virtual bool scatter(
        const ray &r_in, const HitRecord &rec, Color &attenuation, ray &scattered) const
    {
        return false;
    }
    )
};