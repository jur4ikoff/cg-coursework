#pragma once

#include <iostream>
#include <QPixmap>

class Drawer
{
public:
    Drawer(QPixmap &pixmap) : _pixmap(pixmap)
    {
    }

    void draw();

    ~Drawer()
    {
    }

private:
    QPixmap &_pixmap;
};
