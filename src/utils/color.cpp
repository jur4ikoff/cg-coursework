#include "color.h"

double linear_to_gamma(double linear_component) {
  if (linear_component > 0) {
    return std::sqrt(linear_component);
  }
  return 0;
}

void write_color(std::ostream &out, const Color &pixel_color) {
  double r = pixel_color.x();
  double g = pixel_color.y();
  double b = pixel_color.z();

  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);
  int rbyte = int(255.999 * r);
  int gbyte = int(255.999 * g);
  int bbyte = int(255.999 * b);

  out << rbyte << ' ' << gbyte << ' ' << bbyte << "\n";
}

/**
 * @brief Функция трансформирует цвет из самописного Color в Qcolor, а также
 * переводит из формата [0, 1] в формате [0, 255]
 */
QColor get_QColor(const Color &pixel_color) {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);

  const Interval intensity(0.000, 0.999);
  int rbyte = int(256 * intensity.clamp(r));
  int gbyte = int(256 * intensity.clamp(g));
  int bbyte = int(256 * intensity.clamp(b));

  return QColor(rbyte, gbyte, bbyte);
}

QRgb get_QRgb(const Color &color) {
  QRgb q_rgb_color = qRgb(color.x(), color.y(), color.z());
  return q_rgb_color;
}