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

        QImage image = matrix.to_QImage();
        _pixmap = QPixmap::fromImage(image);
    }

    ~QtDrawer()
    {
    }

private:
    QPixmap &_pixmap;
};
