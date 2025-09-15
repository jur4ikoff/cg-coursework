#pragma once

#include <iostream>
#include <QPixmap>

class Drawer
{
public:
    Drawer(QPixmap &pixmap) : _pixmap(pixmap)
    {
    }

    void draw(QColor color)
    {
        // qDebug() << _pixmap.size();
        // for int(j = 0; j < )
        _pixmap.fill(color);
        qDebug() << &_pixmap;
    }

    ~Drawer()
    {
    }

private:
    QPixmap &_pixmap;
};
