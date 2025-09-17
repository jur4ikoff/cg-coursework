#pragma once

#include "color_matrix.h"

#include <iostream>
#include <QPixmap>

class QtDrawer
{
public:
    QtDrawer(QPixmap &pixmap) : _pixmap(pixmap)
    {
    }

    void draw(const ColorMatrix &matrix)
    {
        QImage image = _pixmap.toImage();
        qDebug() << 1;
        _pixmap = QPixmap::fromImage(image);
    }

    ~QtDrawer()
    {
    }

private:
    QPixmap &_pixmap;
};
