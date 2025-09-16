#include "color.h"

void write_color(std::ostream &out, const Color &pixel_color)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    out << rbyte << ' ' << gbyte << ' ' << bbyte << "\n";
}

/**
 * @brief Функция трансформирует цвет из самописного Color в Qcolor, а также переводит из формата [0, 1] в формате [0, 255]
 */
QColor get_QColor(const Color &pixel_color)
{
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    int rbyte = int(255.999 * r);
    int gbyte = int(255.999 * g);
    int bbyte = int(255.999 * b);

    return QColor(rbyte, gbyte, bbyte);
}