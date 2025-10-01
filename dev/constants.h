#pragma once

#include "interval.h"

#include <cmath>
#include <iostream>
#include <memory>

using std::make_shared;
using std::shared_ptr;

inline double degrees_to_radians(double degrees)
{
    return degrees * M_PI / 180.0;
}