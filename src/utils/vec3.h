#pragma once

#include "constants.h"
#include <cmath>
#include <iostream>

class Vec3;

class Vec3 {
public:
  double vector[3];

  Vec3() : vector{0, 0, 0} {}
  Vec3(double e0, double e1, double e2) : vector{e0, e1, e2} {}

  double x() const { return vector[0]; }
  double y() const { return vector[1]; }
  double z() const { return vector[2]; }

  Vec3 operator-() const { return Vec3(-vector[0], -vector[1], -vector[2]); }
  double operator[](int i) const { return vector[i]; }
  double &operator[](int i) { return vector[i]; }

  Vec3 &operator+=(const Vec3 &other) {
    vector[0] += other.vector[0];
    vector[1] += other.vector[1];
    vector[2] += other.vector[2];

    return *this;
  }

  Vec3 &operator+=(const int &n) {
    vector[0] += n;
    vector[1] += n;
    vector[2] += n;

    return *this;
  }

  Vec3 &operator/=(double k) { return *this *= 1 / k; }

  Vec3 operator/(double k) const {
    Vec3 res(*this);
    return res *= 1 / k;
  }

  Vec3 operator/(const Vec3 &other) const {
    return Vec3(this->vector[0] / other.vector[0],
                this->vector[1] / other.vector[1],
                this->vector[2] / other.vector[2]);
  }

  Vec3 operator*(const Vec3 &other) {
    return Vec3(this->vector[0] * other.vector[0],
                this->vector[1] * other.vector[1],
                this->vector[2] * other.vector[2]);
  }

  Vec3 &operator*=(double k) {
    vector[0] *= k;
    vector[1] *= k;
    vector[2] *= k;

    return *this;
  }

  Vec3 operator*(double k) {
    Vec3 res(*this);
    res *= k;

    return res;
  }

  double length() const { return std::sqrt(length_squared()); }

  double length_squared() const {
    return vector[0] * vector[0] + vector[1] * vector[1] +
           vector[2] * vector[2];
  }

  static Vec3 random() {
    return Vec3(random_double(), random_double(), random_double());
  }

  static Vec3 random(double min, double max) {
    return Vec3(random_double(min, max), random_double(min, max),
                random_double(min, max));
  }

  static Vec3 random_unit_vector() {
    while (true) {
      Vec3 p = Vec3::random(-1, 1);
      double lensq = p.length_squared();

      if (1e-160 < lensq && lensq <= 1)
        return p / sqrt(lensq);
    }
  }

  static Vec3 random_on_hemisphere(const Vec3 &normal) {
    Vec3 on_unit_sphere = random_unit_vector();

    if (Vec3::dot(on_unit_sphere, normal) > 0.0)
      return on_unit_sphere;
    return -on_unit_sphere;
  }

  static double dot(const Vec3 &u, const Vec3 &other) {
    return u.vector[0] * other.vector[0] + u.vector[1] * other.vector[1] +
           u.vector[2] * other.vector[2];
  }

  bool near_zero() const {
    auto s = 1e-8;
    return (std::fabs(vector[0]) < s) && (std::fabs(vector[1]) < s) &&
           (std::fabs(vector[2]) < s);
  }
};

inline std::ostream &operator<<(std::ostream &out, const Vec3 &other) {
  return out << other.vector[0] << ' ' << other.vector[1] << ' '
             << other.vector[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &other) {
  return Vec3(u.vector[0] + other.vector[0], u.vector[1] + other.vector[1],
              u.vector[2] + other.vector[2]);
}

inline Vec3 operator+(const Vec3 &u, double n) {
  return Vec3(u.vector[0] + n, u.vector[1] + n, u.vector[2] + n);
}

inline Vec3 operator+(double n, const Vec3 &u) {
  return Vec3(u.vector[0] + n, u.vector[1] + n, u.vector[2] + n);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &other) {
  return Vec3(u.vector[0] - other.vector[0], u.vector[1] - other.vector[1],
              u.vector[2] - other.vector[2]);
}

inline Vec3 operator*(double k, const Vec3 &other) {
  return Vec3(k * other.vector[0], k * other.vector[1], k * other.vector[2]);
}

inline Vec3 operator*(const Vec3 &other, double k) { return k * other; }

inline Vec3 cross(const Vec3 &u, const Vec3 &other) {
  return Vec3(u.vector[1] * other.vector[2] - u.vector[2] * other.vector[1],
              u.vector[2] * other.vector[0] - u.vector[0] * other.vector[2],
              u.vector[0] * other.vector[1] - u.vector[1] * other.vector[0]);
}

// Приведение к едничному вектору
inline Vec3 unit_vector(const Vec3 &vector) { return vector / vector.length(); }

inline Vec3 reflect(const Vec3 &v, const Vec3 &n) {
  return v - 2 * Vec3::dot(v, n) * n;
}

inline Vec3 refract(const Vec3 &uv, const Vec3 &normal, double etai_over_etat) {
  auto cos_theta = std::fmin(Vec3::dot(-uv, normal), 1.0);
  Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * normal);
  Vec3 r_out_parralel =
      -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * normal;
  return r_out_perp + r_out_parralel;
}

inline Vec3 random_in_unit_disk() {
  while (true) {
    auto p = Vec3(random_double(-1, 1), random_double(-1, 1), 0);
    if (p.length_squared() < 1)
      return p;
  }
}

using Point3 = Vec3;
using Size = Vec3;