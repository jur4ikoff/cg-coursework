#pragma once

#include "color_matrix.h"

#include <iostream>
#include <QPixmap>

class Drawer
{
public:
    Drawer(QPixmap &pixmap) : _pixmap(pixmap)
    {
    }

    ColorMatrix draw();

    ~Drawer()
    {
    }

private:
    QPixmap &_pixmap;
};
