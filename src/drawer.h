#pragma once

#include "color_matrix.h"

#include <iostream>
#include <QSize>

class Drawer
{
public:
    Drawer() {}

    ColorMatrix draw(QSize size);

    ~Drawer()
    {
    }
};
