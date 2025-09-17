#pragma once

#include "color_matrix.h"

#include <iostream>
#include <QSize>

class RayTraicer
{
public:
    RayTraicer() {}

    ColorMatrix draw(QSize size);

    ~RayTraicer()
    {
    }
};
