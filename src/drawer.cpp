#include "drawer.hpp"

void Drawer::draw()
{
    QImage image = _pixmap.toImage();
    QSize size = _pixmap.size();

    for (size_t j = 0; j < size.height(); j++)
    {
        for (size_t i = 0; i < size.width(); i++)
        {
            double r = double(i) / (size.width() - 1);
            double g = double(j) / (size.height() - 1);
            double b = 0.0;

            int ir = int(255.999 * r);
            int ig = int(255.999 * g);
            int ib = int(255.999 * b);

            QColor color(ir, ig, ib);
            image.setPixelColor(i, j, color);
        }
    }
    _pixmap = QPixmap::fromImage(image);
}