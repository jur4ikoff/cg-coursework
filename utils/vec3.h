#pragma once

#include <cmath>
#include <iostream>

class MyVec3
{
public:
    double vector[3];

    MyVec3() : vector{0, 0, 0} {}
    MyVec3(double e0, double e1, double e2) : vector{e0, e1, e2} {}

    double x() const { return vector[0]; }
    double y() const { return vector[1]; }
    double z() const { return vector[2]; }

    MyVec3 operator-() const { return MyVec3(-vector[0], -vector[1], -vector[2]); }
    double operator[](int i) const { return vector[i]; }
    double &operator[](int i) { return vector[i]; }

    MyVec3 &operator+=(const MyVec3 &other)
    {
        vector[0] += other.vector[0];
        vector[1] += other.vector[1];
        vector[2] += other.vector[2];

        return *this;
    }

    MyVec3 &operator+=(const int &n)
    {
        vector[0] += n;
        vector[1] += n;
        vector[2] += n;

        return *this;
    }

    MyVec3 &operator*=(double k)
    {
        vector[0] *= k;
        vector[1] *= k;
        vector[2] *= k;

        return *this;
    }

    MyVec3 &operator/=(double k)
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

inline std::ostream &operator<<(std::ostream &out, const MyVec3 &other)
{
    return out << other.vector[0] << ' ' << other.vector[1] << ' ' << other.vector[2];
}

inline MyVec3 operator+(const MyVec3 &u, const MyVec3 &other)
{
    return MyVec3(u.vector[0] + other.vector[0], u.vector[1] + other.vector[1], u.vector[2] + other.vector[2]);
}

// inline Vec3 operator+(int n, const Vec3 &u)
// {
//     return Vec3(u.vector[0] + n, u.vector[1] + n, u.vector[2] + n);
// }

inline MyVec3 operator+(const MyVec3 &u, double n)
{
    return MyVec3(u.vector[0] + n, u.vector[1] + n, u.vector[2] + n);
}

inline MyVec3 operator+(double n, const MyVec3 &u)
{
    return MyVec3(u.vector[0] + n, u.vector[1] + n, u.vector[2] + n);
}



inline MyVec3 operator-(const MyVec3 &u, const MyVec3 &other)
{
    return MyVec3(u.vector[0] - other.vector[0], u.vector[1] - other.vector[1], u.vector[2] - other.vector[2]);
}

inline MyVec3 operator*(const MyVec3 &u, const MyVec3 &other)
{
    return MyVec3(u.vector[0] * other.vector[0], u.vector[1] * other.vector[1], u.vector[2] * other.vector[2]);
}

inline MyVec3 operator*(double k, const MyVec3 &other)
{
    return MyVec3(k * other.vector[0], k * other.vector[1], k * other.vector[2]);
}

inline MyVec3 operator*(const MyVec3 &other, double k)
{
    return k * other;
}

inline MyVec3 operator/(const MyVec3 &other, double k)
{
    return (1 / k) * other;
}

inline double dot(const MyVec3 &u, const MyVec3 &other)
{
    return u.vector[0] * other.vector[0] + u.vector[1] * other.vector[1] + u.vector[2] * other.vector[2];
}

inline MyVec3 cross(const MyVec3 &u, const MyVec3 &other)
{
    return MyVec3(u.vector[1] * other.vector[2] - u.vector[2] * other.vector[1],
                u.vector[2] * other.vector[0] - u.vector[0] * other.vector[2],
                u.vector[0] * other.vector[1] - u.vector[1] * other.vector[0]);
}

// Приведение к едничному вектору
inline MyVec3 unit_vector(const MyVec3 &vector)
{
    return vector / vector.length();
}

using MyPoint3 = MyVec3;
using Size = MyVec3;