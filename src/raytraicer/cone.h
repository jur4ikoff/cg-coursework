// #ifndef CONE_H
// #define CONE_H

// #include "hittable.h"
// #include <cmath>
// #include <memory>

// class cone : public hittable {
//   public:
//     // base_center — центр основания (Y = y_min)
//     // radius — радиус основания
//     // height — высота конуса (вершина будет в base_center.y() + height)
//     cone(const point3& base_center, double radius, double height, shared_ptr<material> mat)
//       : center_x(base_center.x()), 
//         center_z(base_center.z()),
//         y_min(base_center.y()),
//         y_max(base_center.y() + height),
//         radius(radius),
//         mat(mat)
//     {
//         double r = radius;
//         point3 min(center_x - r, y_min, center_z - r);
//         point3 max(center_x + r, y_max, center_z + r);
//         bbox = aabb(min, max);
//     }

//     aabb bounding_box() const override { return bbox; }

//     bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
//         // Переносим луч в локальную систему (ось конуса — Y, основание в y=0)
//         double ox = r.origin().x() - center_x;
//         double oy = r.origin().y() - y_min;
//         double oz = r.origin().z() - center_z;
//         double dx = r.direction().x();
//         double dy = r.direction().y();
//         double dz = r.direction().z();

//         const double h = y_max - y_min; // высота
//         if (h < 1e-8) return false;     // вырожденный конус

//         const double k = radius / h;    // наклон образующей
//         const double k2 = k * k;

//         // Коэффициенты квадратного уравнения: a*t² + b*t + c = 0
//         double a = dx*dx + dz*dz - k2*dy*dy;
//         double b = 2.0 * (dx*ox + dz*oz - k2*dy*oy);
//         double c = ox*ox + oz*oz - k2*oy*oy;

//         // Список кандидатов на t
//         double candidates[2];
//         int count = 0;

//         const double EPS = 1e-12;

//         if (std::fabs(a) < EPS) {
//             // Луч параллелен боковой поверхности → линейное уравнение
//             if (std::fabs(b) > EPS) {
//                 double t = -c / b;
//                 if (ray_t.contains(t)) {
//                     double y = oy + t * dy;
//                     if (y >= 0.0 && y <= h) {
//                         candidates[count++] = t;
//                     }
//                 }
//             }
//         } else {
//             double discriminant = b*b - 4*a*c;
//             if (discriminant >= 0) {
//                 double sqrt_disc = std::sqrt(discriminant);
//                 double t0 = (-b - sqrt_disc) / (2*a);
//                 double t1 = (-b + sqrt_disc) / (2*a);

//                 double y0 = oy + t0 * dy;
//                 double y1 = oy + t1 * dy;

//                 if (ray_t.contains(t0) && y0 >= 0.0 && y0 <= h) {
//                     candidates[count++] = t0;
//                 }
//                 if (ray_t.contains(t1) && y1 >= 0.0 && y1 <= h) {
//                     candidates[count++] = t1;
//                 }
//             }
//         }

//         // Выбираем ближайшее пересечение
//         double t = -1.0;
//         if (count > 0) {
//             t = candidates[0];
//             if (count == 2 && candidates[1] < t) {
//                 t = candidates[1];
//             }
//         }

//         if (t >= 0.0) {
//             // Успешное пересечение с боковой поверхностью или вершиной
//             point3 p = r.at(t);
//             double y_local = p.y() - y_min;

//             rec.t = t;
//             rec.p = p;
//             rec.mat = mat;

//             // Особый случай: вершина (y_local == h)
//             if (std::fabs(y_local - h) < 1e-5) {
//                 // В вершине нормаль не определена — используем (0,1,0)
//                 rec.normal = vec3(0, 1, 0);
//                 rec.front_face = true;
//             } else {
//                 // Обычная нормаль через градиент F = x² + z² - (k*y)²
//                 double x = p.x() - center_x;
//                 double z = p.z() - center_z;
//                 double y = y_local;
//                 vec3 outward_normal(x, -k2 * y, z);
//                 // Нормаль направлена НАРУЖУ
//                 rec.set_face_normal(r, unit_vector(outward_normal));
//             }

//             // Защита от shadow acne
//             rec.p += rec.normal * 1e-4;
//             return true;
//         }

//         // Пересечение с основанием (круг в y = y_min)
//         if (std::fabs(r.direction().y()) > 1e-8) {
//             double t_base = (y_min - r.origin().y()) / r.direction().y();
//             if (ray_t.contains(t_base)) {
//                 point3 p = r.at(t_base);
//                 double dx_base = p.x() - center_x;
//                 double dz_base = p.z() - center_z;
//                 if (dx_base*dx_base + dz_base*dz_base <= radius*radius) {
//                     rec.t = t_base;
//                     rec.p = p;
//                     rec.mat = mat;
//                     rec.normal = vec3(0, -1, 0); // вниз
//                     rec.front_face = true;
//                     rec.p += rec.normal * 1e-4;
//                     return true;
//                 }
//             }
//         }

//         return false;
//     }

//   private:
//     double center_x, center_z;
//     double y_min, y_max;
//     double radius;
//     shared_ptr<material> mat;
//     aabb bbox;
// };

// #endif



#ifndef CONE_H
#define CONE_H

#include "hittable.h"
#include <cmath>

class cone : public hittable {
  public:
    // center: центр основания (основание лежит в плоскости y = center.y())
    // radius: радиус основания
    // height: высота конуса (вершина будет в center.y() + height)
    cone(const point3& center, double radius, double height, shared_ptr<material> mat)
      : cx(center.x()), cz(center.z()),
        y_min(center.y()), y_max(center.y() + height),
        radius(radius), mat(mat)
    {
        // AABB: охватывает весь конус
        auto r = radius;
        bbox = aabb(
            point3(cx - r, y_min, cz - r),
            point3(cx + r, y_max, cz + r)
        );
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        // Переносим всё так, чтобы основание было в y=0
        double ox = r.origin().x() - cx;
        double oy = r.origin().y() - y_min;
        double oz = r.origin().z() - cz;
        double dx = r.direction().x();
        double dy = r.direction().y();
        double dz = r.direction().z();

        const double h = y_max - y_min;
        if (h <= 0 || radius <= 0) return false;

        // Уравнение конуса: x² + z² = (radius * (1 - y/h))², где 0 ≤ y ≤ h
        // Перепишем как: x² + z² - (radius² / h²) * (h - y)² = 0
        double k = radius / h;
        double a = dx*dx + dz*dz - k*k*dy*dy;
        double b = 2*(dx*ox + dz*oz + k*k*dy*(h - oy));
        double c = ox*ox + oz*oz - k*k*(h - oy)*(h - oy);

        double discriminant = b*b - 4*a*c;
        if (discriminant < 0) return false;

        double sqrt_disc = std::sqrt(discriminant);
        double t1 = (-b - sqrt_disc) / (2*a);
        double t2 = (-b + sqrt_disc) / (2*a);

        double t = -1.0;

        // Проверяем t1
        if (ray_t.contains(t1)) {
            double y = oy + t1 * dy;
            if (y >= 0 && y <= h) {
                t = t1;
            }
        }

        // Если t1 не подошёл — пробуем t2
        if (t < 0 && ray_t.contains(t2)) {
            double y = oy + t2 * dy;
            if (y >= 0 && y <= h) {
                t = t2;
            }
        }

        if (t < 0) {
            return hit_base(r, ray_t, rec);
        }

        // Заполняем hit_record
        point3 p = r.at(t);
        rec.t = t;
        rec.p = p;
        rec.mat = mat;

        double y_local = p.y() - y_min;
        if (y_local >= h - 1e-5) {
            // Вершина — задаём нормаль вверх
            rec.normal = vec3(0, 1, 0);
            rec.front_face = true;
        } else {
            // Нормаль к боковой поверхности
            double x = p.x() - cx;
            double z = p.z() - cz;
            double y_from_top = h - y_local; // расстояние от вершины
            double r_local = k * y_from_top;

            // Производная: dF/dx = 2x, dF/dy = 2k² y_from_top, dF/dz = 2z
            vec3 outward_normal(x, k*k * y_from_top, z);
            outward_normal = unit_vector(outward_normal);
            rec.set_face_normal(r, outward_normal);
        }

        rec.p += rec.normal * 1e-4;
        return true;
    }

    aabb bounding_box() const override { return bbox; }

  private:
    bool hit_base(const ray& r, interval ray_t, hit_record& rec) const {
        if (std::fabs(r.direction().y()) < 1e-8) return false;

        double t = (y_min - r.origin().y()) / r.direction().y();
        if (!ray_t.contains(t)) return false;

        point3 p = r.at(t);
        double dx = p.x() - cx;
        double dz = p.z() - cz;
        if (dx*dx + dz*dz <= radius*radius) {
            rec.t = t;
            rec.p = p;
            rec.mat = mat;
            rec.normal = vec3(0, -1, 0); // вниз
            rec.front_face = true;
            rec.p += rec.normal * 1e-4;
            return true;
        }
        return false;
    }

    double cx, cz;
    double y_min, y_max;
    double radius;
    shared_ptr<material> mat;
    aabb bbox;
};

#endif