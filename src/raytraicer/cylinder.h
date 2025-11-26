// #ifndef CYLINDER_H
// #define CYLINDER_H

// #include "hittable.h"
// #include "quad.h" // для дисков-оснований (опционально)
// #include <cmath>
// #include <memory>

// class cylinder : public hittable
// {
// public:
//     cylinder() {}
//     cylinder(double radius, double y_min, double y_max, shared_ptr<material> mat)
//         : radius(radius), y_min(y_min), y_max(y_max), mat(mat)
//     {
//         // Убедимся, что y_min < y_max
//         if (y_min > y_max)
//             std::swap(y_min, y_max);

//         set_bounding_box();
//     }

//     void set_bounding_box()
//     {
//         auto r = radius;
//         point3 min(-r, y_min, -r);
//         point3 max(r, y_max, r);
//         bbox = aabb(min, max);
//     }

//     aabb bounding_box() const override { return bbox; }

//     bool hit(const ray &r, interval ray_t, hit_record &rec) const override
//     {
//         // Пересечение с бесконечным цилиндром вдоль оси Y: x² + z² = r²
//         auto oc = r.origin() - point3(0, 0, 0);
//         auto rd = r.direction();

//         // Квадратное уравнение: a*t² + b*t + c = 0
//         auto a = rd.x() * rd.x() + rd.z() * rd.z();
//         if (a < 1e-12) // Луч почти параллелен оси цилиндра
//             return false;

//         auto b = 2 * (oc.x() * rd.x() + oc.z() * rd.z());
//         auto c = oc.x() * oc.x() + oc.z() * oc.z() - radius * radius;

//         auto discriminant = b * b - 4 * a * c;
//         if (discriminant < 0)
//             return false;

//         auto sqrt_disc = std::sqrt(discriminant);
//         auto t0 = (-b - sqrt_disc) / (2 * a);
//         auto t1 = (-b + sqrt_disc) / (2 * a);

//         // Рассматриваем только точки в пределах [y_min, y_max]
//         bool hit0 = false, hit1 = false;
//         auto y0 = r.at(t0).y();
//         auto y1 = r.at(t1).y();

//         if (y0 >= y_min && y0 <= y_max && ray_t.contains(t0))
//             hit0 = true;
//         if (y1 >= y_min && y1 <= y_max && ray_t.contains(t1))
//             hit1 = true;

//         double t;
//         vec3 point;
//         if (hit0)
//         {
//             t = t0;
//             point = r.at(t0);
//         }
//         else if (hit1)
//         {
//             t = t1;
//             point = r.at(t1);
//         }
//         else
//         {
//             // Попробуем пересечение с основаниями (дисками)
//             return hit_caps(r, ray_t, rec);
//         }

//         // Нормаль к боковой поверхности: проекция на плоскость XZ, нормализованная
//         vec3 outward_normal = vec3(point.x(), 0, point.z()) / radius;
//         rec.set_face_normal(r, outward_normal);
//         // rec.t = t;
//         // rec.p = point;
//         // rec.mat = mat;

//         // // UV-координаты: v = y, u = угол (от 0 до 1)
//         // rec.u = (std::atan2(point.z(), point.x()) + pi) / (2 * pi);
//         // rec.v = (point.y() - y_min) / (y_max - y_min);
//         rec.t = t;
//         rec.p = point;
//         rec.mat = mat;
//         rec.set_face_normal(r, outward_normal);

//         rec.u = (std::atan2(point.z(), point.x()) + pi) / (2 * pi);
//         rec.v = (point.y() - y_min) / (y_max - y_min);

//         // Смещаем точку, чтобы избежать self-intersection
//         rec.p += rec.normal * 1e-4;

//         return true;
//     }

// private:
//     // bool hit_caps(const ray& r, interval ray_t, hit_record& rec) const {
//     //     // Нижнее основание (y = y_min)
//     //     auto normal_bottom = vec3(0, -1, 0);
//     //     auto t_bottom = (y_min - r.origin().y()) / r.direction().y();
//     //     if (std::fabs(r.direction().y()) > 1e-8 && ray_t.contains(t_bottom)) {
//     //         auto p = r.at(t_bottom);
//     //         if (p.x() * p.x() + p.z() * p.z() <= radius * radius) {
//     //             rec.t = t_bottom;
//     //             rec.p = p;
//     //             rec.mat = mat;
//     //             rec.set_face_normal(r, normal_bottom);
//     //             rec.u = p.x() / radius;
//     //             rec.v = p.z() / radius;
//     //             return true;
//     //         }
//     //     }

//     //     // Верхнее основание (y = y_max)
//     //     auto normal_top = vec3(0, 1, 0);
//     //     auto t_top = (y_max - r.origin().y()) / r.direction().y();
//     //     if (std::fabs(r.direction().y()) > 1e-8 && ray_t.contains(t_top)) {
//     //         auto p = r.at(t_top);
//     //         if (p.x() * p.x() + p.z() * p.z() <= radius * radius) {
//     //             rec.t = t_top;
//     //             rec.p = p;
//     //             rec.mat = mat;
//     //             rec.set_face_normal(r, normal_top);
//     //             rec.u = p.x() / radius;
//     //             rec.v = p.z() / radius;
//     //             return true;
//     //         }
//     //     }

//     //     return false;
//     // }

//     bool hit_caps(const ray &r, interval ray_t, hit_record &rec) const
//     {
//         static constexpr double pi = 3.1415926535897932385;

//         // Нижнее основание (y = y_min)
//         if (std::fabs(r.direction().y()) > 1e-8)
//         {
//             auto t_bottom = (y_min - r.origin().y()) / r.direction().y();
//             if (ray_t.contains(t_bottom))
//             {
//                 auto p = r.at(t_bottom);
//                 if (p.x() * p.x() + p.z() * p.z() <= radius * radius)
//                 {
//                     rec.t = t_bottom;
//                     rec.p = p;
//                     rec.mat = mat;
//                     rec.set_face_normal(r, vec3(0, -1, 0));

//                     // Вариант 1: константные UV (без артефактов)
//                     rec.u = 0.5;
//                     rec.v = 0.5;

//                     // Вариант 2: радиальные UV (если нужно для текстуры)
//                     /*
//                     auto r = sqrt(p.x()*p.x() + p.z()*p.z());
//                     if (r > 1e-8) {
//                         rec.u = p.x() / r;
//                         rec.v = p.z() / r;
//                     } else {
//                         rec.u = 0;
//                         rec.v = 0;
//                     }
//                     */

//                     rec.p += rec.normal * 1e-4;
//                     return true;
//                 }
//             }
//         }

//         // Верхнее основание (y = y_max)
//         if (std::fabs(r.direction().y()) > 1e-8)
//         {
//             auto t_top = (y_max - r.origin().y()) / r.direction().y();
//             if (ray_t.contains(t_top))
//             {
//                 auto p = r.at(t_top);
//                 if (p.x() * p.x() + p.z() * p.z() <= radius * radius)
//                 {
//                     rec.t = t_top;
//                     rec.p = p;
//                     rec.mat = mat;
//                     rec.set_face_normal(r, vec3(0, 1, 0));

//                     rec.u = 0.5;
//                     rec.v = 0.5;

//                     rec.p += rec.normal * 1e-4;
//                     return true;
//                 }
//             }
//         }

//         return false;
//     }

// private:
//     static constexpr double pi = 3.1415926535897932385;
//     double radius;
//     double y_min, y_max;
//     shared_ptr<material> mat;
//     aabb bbox;
// };

// #endif

#ifndef CYLINDER_H
#define CYLINDER_H

#include "hittable.h"
#include <cmath>
#include <memory>

class cylinder : public hittable
{
public:
    // Конструктор: цилиндр вдоль оси Y от y0 до y1, радиусом r
    cylinder(const point3 &center, double radius, double y0, double y1, shared_ptr<material> mat)
        : center(center.x(), 0, center.z()), // x и z — центр оси, y игнорируется
          radius(radius),
          y_min(std::fmin(y0, y1)),
          y_max(std::fmax(y0, y1)),
          mat(mat)
    {
        // Ограничивающий AABB: цилиндр вписан в коробку
        auto r = radius;
        point3 min(center.x() - r, y_min, center.z() - r);
        point3 max(center.x() + r, y_max, center.z() + r);
        bbox = aabb(min, max);
    }

    aabb bounding_box() const override { return bbox; }

    bool hit(const ray &r, interval ray_t, hit_record &rec) const override
    {
        // Смещаем луч так, чтобы цилиндр был вдоль оси Y с центром в (0,*,0)
        ray shifted_ray(r.origin() - center, r.direction(), r.time());

        auto oc = shifted_ray.origin();
        auto rd = shifted_ray.direction();

        // Пересечение с бесконечным цилиндром: x² + z² = r²
        auto a = rd.x() * rd.x() + rd.z() * rd.z();
        if (a < 1e-12)
            return false; // Луч почти параллелен оси цилиндра

        auto b = oc.x() * rd.x() + oc.z() * rd.z();
        auto c = oc.x() * oc.x() + oc.z() * oc.z() - radius * radius;

        auto discriminant = b * b - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);
        auto t1 = (-b - sqrtd) / a;
        auto t2 = (-b + sqrtd) / a;

        // Пытаемся найти пересечение с боковой поверхностью в пределах [y_min, y_max]
        bool hit_side = false;
        double t = 0;
        point3 p;

        if (ray_t.contains(t1))
        {
            p = shifted_ray.at(t1);
            if (p.y() >= y_min && p.y() <= y_max)
            {
                hit_side = true;
                t = t1;
            }
        }
        if (!hit_side && ray_t.contains(t2))
        {
            p = shifted_ray.at(t2);
            if (p.y() >= y_min && p.y() <= y_max)
            {
                hit_side = true;
                t = t2;
            }
        }

        if (hit_side)
        {
            // Восстанавливаем мировые координаты
            p += center;

            vec3 outward_normal(p.x() - center.x(), 0, p.z() - center.z());
            outward_normal /= radius;

            rec.t = t;
            rec.p = p;
            rec.set_face_normal(r, outward_normal);
            // std::cout << "Normal: " << rec.normal << std::endl;
            rec.mat = mat;

            // UV: u — угол, v — высота (опционально, можно убрать)
            rec.u = std::atan2(outward_normal.z(), outward_normal.x()) / (2 * M_PI) + 0.5;
            rec.v = (p.y() - y_min) / (y_max - y_min);

            return true;
        }

        // Иначе — пробуем крышки
        return hit_caps(r, ray_t, rec);
    }

private:
    bool hit_caps(const ray &r, interval ray_t, hit_record &rec) const
    {
        // Верхняя крышка
        if (std::fabs(r.direction().y()) > 1e-8)
        {
            auto t = (y_max - r.origin().y()) / r.direction().y();
            if (ray_t.contains(t))
            {
                auto p = r.at(t);
                auto dx = p.x() - center.x();
                auto dz = p.z() - center.z();
                if (dx * dx + dz * dz <= radius * radius)
                {
                    rec.t = t;
                    rec.p = p;
                    rec.mat = mat;
                    // rec.set_face_normal(r, vec3(0, 1, 0)); // outward_normal
                    rec.normal = vec3(0, 1, 0);
                    rec.u = 0.0;
                    rec.v = 0.0;
                    // Смещаем ПОСЛЕ установки нормали:
                    rec.p += rec.normal * 1e-4;
                    return true;
                }
            }
        }

        // Нижняя крышка
        if (std::fabs(r.direction().y()) > 1e-8)
        {
            auto t = (y_min - r.origin().y()) / r.direction().y();
            if (ray_t.contains(t))
            {
                auto p = r.at(t);
                auto dx = p.x() - center.x();
                auto dz = p.z() - center.z();
                if (dx * dx + dz * dz <= radius * radius)
                {
                    rec.t = t;
                    rec.p = p;
                    rec.mat = mat;
                    rec.set_face_normal(r, vec3(0, -1, 0)); // outward_normal
                    rec.u = 0.0;
                    rec.v = 0.0;
                    rec.p += rec.normal * 1e-4;
                    return true;
                }
            }
        }

        return false;
    }

private:
    point3 center; // центр оси цилиндра (x, 0, z)
    double radius;
    double y_min, y_max;
    shared_ptr<material> mat;
    aabb bbox;
};



#endif