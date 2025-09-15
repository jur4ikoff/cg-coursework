#include "drawer.hpp"
#include "color.h"
#include "vec3.h"

void Drawer::draw()
{
    QImage image = _pixmap.toImage();
    QSize size = _pixmap.size();

    for (size_t j = 0; j < size.height(); j++)
    {
        for (size_t i = 0; i < size.width(); i++)
        {
            Color color(double(i) / (size.width() - 1), double(j) / (size.height() - 1), 0);
            QColor normalize_color = get_QColor(color);

            image.setPixelColor(i, j, normalize_color);
        }
    }
    _pixmap = QPixmap::fromImage(image);
}