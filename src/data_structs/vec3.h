#ifndef VEC3_H
#define VEC3_H

#include "constants.h"

class Vec3 {
public:
  double e[3];

  Vec3() : e{0, 0, 0} {}
  Vec3(double e0, double e1, double e2) : e{e0, e1, e2} {}

  double x() const { return e[0]; }
  double y() const { return e[1]; }
  double z() const { return e[2]; }

  Vec3 operator-() const { return Vec3(-e[0], -e[1], -e[2]); }
  double operator[](int i) const { return e[i]; }
  double &operator[](int i) { return e[i]; }

  Vec3 &operator+=(const Vec3 &v) {
    e[0] += v.e[0];
    e[1] += v.e[1];
    e[2] += v.e[2];
    return *this;
  }

  Vec3 &operator+=(const int &n) {
    e[0] += n;
    e[1] += n;
    e[2] += n;

    return *this;
  }

  Vec3 &operator*=(double t) {
    e[0] *= t;
    e[1] *= t;
    e[2] *= t;
    return *this;
  }

  Vec3 operator*(double k) {
    Vec3 res(*this);
    res *= k;

    return res;
  }

  Vec3 operator/(const Vec3 &other) const {
    return Vec3(this->e[0] / other.e[0], this->e[1] / other.e[1],
                this->e[2] / other.e[2]);
  }

  Vec3 &operator/=(double t) { return *this *= 1 / t; }

  double length() const { return std::sqrt(length_squared()); }

  double length_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }

  bool near_zero() const {
    // Return true if the e is close to zero in all dimensions.
    auto s = 1e-8;
    return (std::fabs(e[0]) < s) && (std::fabs(e[1]) < s) &&
           (std::fabs(e[2]) < s);
  }

  static Vec3 random() {
    return Vec3(random_double(), random_double(), random_double());
  }

  static Vec3 random(double min, double max) {
    return Vec3(random_double(min, max), random_double(min, max),
                random_double(min, max));
  }
};

// Функции переопредления операторов
inline std::ostream &operator<<(std::ostream &out, const Vec3 &other) {
  return out << other.e[0] << ' ' << other.e[1] << ' ' << other.e[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline Vec3 operator+(const Vec3 &u, double n) {
  return Vec3(u.e[0] + n, u.e[1] + n, u.e[2] + n);
}

inline Vec3 operator+(double n, const Vec3 &u) {
  return Vec3(u.e[0] + n, u.e[1] + n, u.e[2] + n);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline Vec3 operator*(double t, const Vec3 &v) {
  return Vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

inline Vec3 operator*(const Vec3 &v, double t) { return t * v; }

inline Vec3 operator/(const Vec3 &v, double t) { return (1 / t) * v; }

inline double dot(const Vec3 &u, const Vec3 &v) {
  return u.e[0] * v.e[0] + u.e[1] * v.e[1] + u.e[2] * v.e[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &v) {
  return Vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
              u.e[2] * v.e[0] - u.e[0] * v.e[2],
              u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline Vec3 unit_vector(const Vec3 &v) { return v / v.length(); }

inline Vec3 random_in_unit_disk() {
  while (true) {
    auto p = Vec3(random_double(-1, 1), random_double(-1, 1), 0);
    if (p.length_squared() < 1)
      return p;
  }
}

inline Vec3 random_unit_vector() {
  while (true) {
    auto p = Vec3::random(-1, 1);
    auto lensq = p.length_squared();
    if (1e-160 < lensq && lensq <= 1.0)
      return p / sqrt(lensq);
  }
}

inline Vec3 random_cosine_direction() {
  auto r1 = random_double();
  auto r2 = random_double();
  auto z = std::sqrt(1 - r2);
  auto phi = 2 * M_PI * r1;
  auto x = std::cos(phi) * std::sqrt(r2);
  auto y = std::sin(phi) * std::sqrt(r2);
  return Vec3(x, y, z);
}

inline Vec3 cosine_sample_hemisphere(const Vec3 &n) {
  // 1. Сэмпл в локальной системе (z = нормаль)
  double r1 = random_double();
  double r2 = random_double();
  double cos_theta = std::sqrt(1 - r2);
  double sin_theta = std::sqrt(r2);
  double phi = 2 * M_PI * r1;

  Vec3 local(sin_theta * std::cos(phi), sin_theta * std::sin(phi), cos_theta);

  // 2. Базис вокруг нормали n
  Vec3 w = unit_vector(n);
  Vec3 a = (std::fabs(w.x()) > 0.9) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
  Vec3 u = unit_vector(cross(a, w));
  Vec3 v = cross(w, u);

  // 3. Преобразование в мировую систему
  return local.x() * u + local.y() * v + local.z() * w;
}

inline Vec3 random_on_hemisphere(const Vec3 &normal) {
  Vec3 on_unit_sphere = random_unit_vector();
  if (dot(on_unit_sphere, normal) > 0.0) // In the same hemisphere as the normal
    return on_unit_sphere;
  else
    return -on_unit_sphere;
}

// зеркальное отражение
inline Vec3 reflect(const Vec3 &v, const Vec3 &n) {
  return v - 2 * dot(v, n) * n;
}

// Преломление закон снелиуса
inline Vec3 refract(const Vec3 &uv, const Vec3 &n, double etai_over_etat) {
  // Косинус угла падения
  auto cos_theta = std::fmin(dot(-uv, n), 1.0);
  Vec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
  Vec3 r_out_parallel =
      -std::sqrt(std::fabs(1.0 - r_out_perp.length_squared())) * n;
  return r_out_perp + r_out_parallel;
}

using Size = Vec3;
using Point3 = Vec3;

#endif
