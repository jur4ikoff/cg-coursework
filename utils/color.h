#pragma once
#include "vec3.h"

#include <QColor>

using Color = MyVec3;

void write_color(std::ostream &out, const Color &pixel_color);
QColor get_QColor(const Color &color);
QRgb get_QRgb(const Color &color);