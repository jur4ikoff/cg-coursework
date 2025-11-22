#pragma once

#include <limits>
const double INF = std::numeric_limits<double>::infinity();
const double PI = M_PI;

class Interval
{
public:
  double min, max;

  Interval() : min(INF), max(-INF) {}
  Interval(double min, double max) : min(min), max(max) {}
  double size() const { return max - min; }

  bool contains(double x) const { return min <= x && x <= max; }

  bool surrounds(double x) const { return min < x && x < max; }

  double clamp(double x) const
  {
    if (x < min)
      return min;
    if (x > max)
      return max;
    return x;
  }

  /**
   * @brief Функция расширяет диапазон текущего интервала и возвращает новый экземпляр
   */
  Interval expand(double delta) const
  {
    auto padding = delta / 2;
    return Interval(min - padding, max + padding);
  }

  static const Interval empty, universe;
};
