#pragma once

#include "color.h"

enum MaterialType
{
    Lambertian_t,
    Metal_t,
    Transparent_t
};

struct MaterialStruct
{

    MaterialType type;

    // Общие параметры
    Color color;                   // для Lambertian, Metal
    double fuzz = 0.0;             // для Metal
    double refraction_index = 1.0; // для Transparent
};