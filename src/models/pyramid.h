#ifndef PYRAMID_H
#define PYRAMID_H

#include "hittable.h"
#include <algorithm>

class Triangle : public Quad
{
public:
    // Конструктор: три вершины A, B, C
    Triangle(const Point3 &A, const Point3 &B, const Point3 &C, shared_ptr<Material> mat)
        : Quad(A, B - A, C - A, mat)
    {
        // Quad уже вычислил normal, w, bbox и т.д.
        // Нам остаётся только переопределить поведение is_interior
    }

    // Переопределяем is_interior для треугольника
    bool is_interior(double a, double b, HitRecord &rec) const override
    {
        // Точка внутри треугольника, если:
        // a >= 0, b >= 0, и a + b <= 1
        if (a < 0 || b < 0 || a + b > 1)
            return false;

        // UV: можно использовать барицентрические координаты
        rec.u = a;
        rec.v = b;
        return true;
    }
};

inline shared_ptr<HittableList> pyramid(const Point3 &base_center, double half_width, double height, shared_ptr<Material> mat)
{
    auto sides = make_shared<HittableList>();

    // Четыре угла основания (Y = 0)
    auto hw = half_width;
    auto h = height;

    Point3 c = base_center;
    Point3 bl(c.x() - hw, c.y(), c.z() - hw); // back-left
    Point3 br(c.x() + hw, c.y(), c.z() - hw); // back-right
    Point3 fr(c.x() + hw, c.y(), c.z() + hw); // front-right
    Point3 fl(c.x() - hw, c.y(), c.z() + hw); // front-left
    Point3 top(c.x(), c.y() + h, c.z());      // вершина

    // Основание (квадрат, направлено вниз)
    sides->add(make_shared<Quad>(bl, br - bl, fl - bl, mat));

    // Боковые грани — треугольники (если у вас есть triangle)
    sides->add(make_shared<Triangle>(bl, br, top, mat));
    sides->add(make_shared<Triangle>(br, fr, top, mat));
    sides->add(make_shared<Triangle>(fr, fl, top, mat));
    sides->add(make_shared<Triangle>(fl, bl, top, mat));

    return sides;
}

// Создаёт пирамиду с произвольным четырёхугольным основанием и вершиной
inline shared_ptr<HittableList> irregular_pyramid(
    const Point3 &v0,   // вершина 0 основания
    const Point3 &v1,   // вершина 1 основания
    const Point3 &v2,   // вершина 2 основания
    const Point3 &v3,   // вершина 3 основания
    const Point3 &apex, // вершина пирамиды
    shared_ptr<Material> mat)
{
    auto sides = make_shared<HittableList>();

    // Основание — Quad (может быть трапецией, ромбом, любым четырёхугольником)
    // Quad строится из точки v0 и векторов к v1 и v3
    sides->add(make_shared<Quad>(v0, v1 - v0, v3 - v0, mat));

    // Боковые грани — треугольники
    sides->add(make_shared<Triangle>(v0, v1, apex, mat));
    sides->add(make_shared<Triangle>(v1, v2, apex, mat));
    sides->add(make_shared<Triangle>(v2, v3, apex, mat));
    sides->add(make_shared<Triangle>(v3, v0, apex, mat));

    return sides;
}

// Создаёт пирамиду с произвольным четырёхугольным основанием и вершиной
inline shared_ptr<HittableList> triangle_pyramid(
    const Point3 &v0,   // вершина 0 основания
    const Point3 &v1,   // вершина 1 основания
    const Point3 &v2,   // вершина 2 основания
    const Point3 &apex, // вершина пирамиды
    shared_ptr<Material> mat)
{
    auto sides = make_shared<HittableList>();

    // Основание — Quad (может быть трапецией, ромбом, любым четырёхугольником)
    // Quad строится из точки v0 и векторов к v1 и v3
    // ⚠️ Важно: основание должно быть планарным и выпуклым!
    sides->add(make_shared<Triangle>(v0, v1, v2, mat));

    // Боковые грани — треугольники
    sides->add(make_shared<Triangle>(v0, v1, apex, mat));
    sides->add(make_shared<Triangle>(v1, v2, apex, mat));
    sides->add(make_shared<Triangle>(v2, v0, apex, mat));

    return sides;
}

#endif