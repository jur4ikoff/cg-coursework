#ifndef CYLINDER_H
#define CYLINDER_H

#include "hittable.h"
#include <cmath>
#include <memory>

class Cylinder : public Hittable
{
public:
    // Конструктор: цилиндр вдоль оси Y от y0 до y1, радиусом r
    Cylinder(const point3 &center, double radius, double y0, double y1, shared_ptr<Material> mat)
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
        bbox = Aaab(min, max);
    }

    Aaab bounding_box() const override { return bbox; }

    bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
    {
        // Смещаем луч так, чтобы цилиндр был вдоль оси Y с центром в (0,*,0)
        Ray shifted_ray(r.origin() - center, r.direction(), r.time());

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

            Vec3 outward_normal(p.x() - center.x(), 0, p.z() - center.z());
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
    bool hit_caps(const Ray &r, Interval ray_t, HitRecord &rec) const
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
                    rec.normal = Vec3(0, 1, 0);
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
                    rec.set_face_normal(r, Vec3(0, -1, 0)); // outward_normal
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
    shared_ptr<Material> mat;
    Aaab bbox;
};



#endif