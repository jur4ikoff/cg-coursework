#ifndef CONE_H
#define CONE_H

#include "hittable.h"
#include <cmath>

class Cone : public Hittable
{
public:
    // center: центр основания (основание лежит в плоскости y = center.y())
    // radius: радиус основания
    // height: высота конуса (вершина будет в center.y() + height)
    Cone(const Point3 &center, double radius, double height, shared_ptr<Material> _mat)
        : cx(center.x()), cz(center.z()),
          y_min(center.y()), y_max(center.y() + height),
          radius(radius) //, mat(mat)
    {
        // AABB: охватывает весь конус
        auto r = radius;
        bbox = Aaab(
            Point3(cx - r, y_min, cz - r),
            Point3(cx + r, y_max, cz + r));

        mat = _mat;
    }

    bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
    {
        // Переносим всё так, чтобы основание было в y=0
        double ox = r.origin().x() - cx;
        double oy = r.origin().y() - y_min;
        double oz = r.origin().z() - cz;
        double dx = r.direction().x();
        double dy = r.direction().y();
        double dz = r.direction().z();

        const double h = y_max - y_min;
        if (h <= 0 || radius <= 0)
            return false;

        // Уравнение конуса: x² + z² = (radius * (1 - y/h))², где 0 ≤ y ≤ h
        // Перепишем как: x² + z² - (radius² / h²) * (h - y)² = 0
        double k = radius / h;
        double a = dx * dx + dz * dz - k * k * dy * dy;
        double b = 2 * (dx * ox + dz * oz + k * k * dy * (h - oy));
        double c = ox * ox + oz * oz - k * k * (h - oy) * (h - oy);

        double discriminant = b * b - 4 * a * c;
        if (discriminant < 0)
            return false;

        double sqrt_disc = std::sqrt(discriminant);
        double t1 = (-b - sqrt_disc) / (2 * a);
        double t2 = (-b + sqrt_disc) / (2 * a);

        double t = -1.0;

        // Проверяем t1
        if (ray_t.contains(t1))
        {
            double y = oy + t1 * dy;
            if (y >= 0 && y <= h)
            {
                t = t1;
            }
        }

        // Если t1 не подошёл — пробуем t2
        if (t < 0 && ray_t.contains(t2))
        {
            double y = oy + t2 * dy;
            if (y >= 0 && y <= h)
            {
                t = t2;
            }
        }

        if (t < 0)
        {
            return hit_base(r, ray_t, rec);
        }

        // Заполняем HitRecord
        Point3 p = r.at(t);
        rec.t = t;
        rec.p = p;
        rec.mat = mat;

        double y_local = p.y() - y_min;
        if (y_local >= h - 1e-5)
        {
            // Вершина — задаём нормаль вверх
            rec.normal = Vec3(0, 1, 0);
            rec.front_face = true;
        }
        else
        {
            // Нормаль к боковой поверхности
            double x = p.x() - cx;
            double z = p.z() - cz;
            double y_from_top = h - y_local; // расстояние от вершины
            double r_local = k * y_from_top;

            // Производная: dF/dx = 2x, dF/dy = 2k² y_from_top, dF/dz = 2z
            Vec3 outward_normal(x, k * k * y_from_top, z);
            outward_normal = unit_vector(outward_normal);
            rec.set_face_normal(r, outward_normal);
        }

        rec.p += rec.normal * 1e-4;
        return true;
    }

    Aaab bounding_box() const override { return bbox; }

private:
    bool hit_base(const Ray &r, Interval ray_t, HitRecord &rec) const
    {
        if (std::fabs(r.direction().y()) < 1e-8)
            return false;

        double t = (y_min - r.origin().y()) / r.direction().y();
        if (!ray_t.contains(t))
            return false;

        Point3 p = r.at(t);
        double dx = p.x() - cx;
        double dz = p.z() - cz;
        if (dx * dx + dz * dz <= radius * radius)
        {
            rec.t = t;
            rec.p = p;
            rec.mat = mat;
            rec.normal = Vec3(0, -1, 0); // вниз
            rec.front_face = true;
            rec.p += rec.normal * 1e-4;
            return true;
        }
        return false;
    }

    double cx, cz;
    double y_min, y_max;
    double radius;
    // shared_ptr<Material> mat;
    Aaab bbox;
};

#endif