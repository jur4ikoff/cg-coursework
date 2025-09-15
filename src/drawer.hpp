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
    void main();

    ~Drawer()
    {
    }

private:
    QPixmap &_pixmap;
};
