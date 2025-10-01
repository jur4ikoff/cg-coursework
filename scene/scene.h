#pragma once

#include "color_matrix.h"

#include <iostream>
#include <QSize>

class Scene
{
public:
    Scene() {}

    ColorMatrix draw(QSize size);

    ~Scene()
    {
    }
};
