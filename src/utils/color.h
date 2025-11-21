#pragma once
#include "vec3.h"

#include <QColor>

using Color = Vec3;

double linear_to_gamma(double linear_component);
void write_color(std::ostream &out, const Color &pixel_color);
QColor get_QColor(const Color &color);
QRgb get_QRgb(const Color &color);