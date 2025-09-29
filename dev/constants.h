#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>

using std::make_shared;
using std::shared_ptr;

const double INF = std::numeric_limits<double>::infinity();
const double PI = M_PI; 

inline double degrees_to_radians(double degrees)
{
    return degrees * M_PI / 180.0;
}