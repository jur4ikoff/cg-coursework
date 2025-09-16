#pragma once

#include <cmath>
#include <iostream>

class Vec3
{
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

    Vec3 &operator+=(const Vec3 &other)
    {
        vector[0] += other.vector[0];
        vector[1] += other.vector[1];
        vector[2] += other.vector[2];

        return *this;
    }

    Vec3 &operator*=(double k)
    {
        vector[0] *= k;
        vector[1] *= k;
        vector[2] *= k;

        return *this;
    }

    Vec3 &operator/=(double k)
    {
        return *this *= 1 / k;
    }

    double length() const
    {
        return std::sqrt(lenght_squared());
    }

    double lenght_squared() const
    {
        return vector[0] * vector[0] + vector[1] * vector[1] + vector[2] * vector[2];
    }
};

inline std::ostream &operator<<(std::ostream &out, const Vec3 &other)
{
    return out << other.vector[0] << ' ' << other.vector[1] << ' ' << other.vector[2];
}

inline Vec3 operator+(const Vec3 &u, const Vec3 &other)
{
    return Vec3(u.vector[0] + other.vector[0], u.vector[1] + other.vector[1], u.vector[2] + other.vector[2]);
}

inline Vec3 operator-(const Vec3 &u, const Vec3 &other)
{
    return Vec3(u.vector[0] - other.vector[0], u.vector[1] - other.vector[1], u.vector[2] - other.vector[2]);
}

inline Vec3 operator*(const Vec3 &u, const Vec3 &other)
{
    return Vec3(u.vector[0] * other.vector[0], u.vector[1] * other.vector[1], u.vector[2] * other.vector[2]);
}

inline Vec3 operator*(double k, const Vec3 &other)
{
    return Vec3(k * other.vector[0], k * other.vector[1], k * other.vector[2]);
}

inline Vec3 operator*(const Vec3 &other, double k)
{
    return k * other;
}

inline Vec3 operator/(const Vec3 &other, double k)
{
    return (1 / k) * other;
}

inline double dot(const Vec3 &u, const Vec3 &other)
{
    return u.vector[0] * other.vector[0] + u.vector[1] * other.vector[1] + u.vector[2] * other.vector[2];
}

inline Vec3 cross(const Vec3 &u, const Vec3 &other)
{
    return Vec3(u.vector[1] * other.vector[2] - u.vector[2] * other.vector[1],
                u.vector[2] * other.vector[0] - u.vector[0] * other.vector[2],
                u.vector[0] * other.vector[1] - u.vector[1] * other.vector[0]);
}

// Приведение к едничному вектору
inline Vec3 unit_vector(const Vec3 &vector)
{
    return vector / vector.length();
}

using Point3 = Vec3;