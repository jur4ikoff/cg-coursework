#pragma once

#include "interval.h"

#include <cmath>
#include <cstdlib>
#include <iostream>

using std::make_shared;
using std::shared_ptr;

// functions

inline double degrees_to_radians(double degrees) {
  return degrees * M_PI / 180.0;
}

inline double random_double() {
  // Возвращает рандомное число в диапазоне [0, 1]
  return std::rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
  // Возвращает рандомное число в диапазоне [min, max]
  return min + (max - min) * random_double();
}
