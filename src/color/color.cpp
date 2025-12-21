#include "color.h"

QColor get_QColor(const Color &pixel_color) {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  if (r != r)
    r = 0.0;
  if (g != g)
    g = 0.0;
  if (b != b)
    b = 0.0;

  // r = linear_to_gamma(r);
  // g = linear_to_gamma(g);
  // b = linear_to_gamma(b);

  const Interval intensity(0.000, 0.999);
  int rbyte = int(256 * intensity.clamp(r));
  int gbyte = int(256 * intensity.clamp(g));
  int bbyte = int(256 * intensity.clamp(b));

  return QColor(rbyte, gbyte, bbyte);
}

QRgb get_QRgb(const Color &clr) {
  QRgb q_rgb_color = qRgb(clr.x(), clr.y(), clr.z());
  return q_rgb_color;
}

void write_color(std::ostream &out, const Color &pixel_color) {
  auto r = pixel_color.x();
  auto g = pixel_color.y();
  auto b = pixel_color.z();

  if (r != r)
    r = 0.0;
  if (g != g)
    g = 0.0;
  if (b != b)
    b = 0.0;

  // Apply a linear to gamma transform for gamma 2
  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);

  // Translate the [0,1] component values to the byte range [0,255].
  static const Interval intensity(0.000, 0.999);
  int rbyte = int(256 * intensity.clamp(r));
  int gbyte = int(256 * intensity.clamp(g));
  int bbyte = int(256 * intensity.clamp(b));

  // Write out the pixel color components.
  out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}