#ifndef PYRAMID_H
#define PYRAMID_H

#include "hittable.h"
#include <algorithm>

class triangle : public quad
{
public:
    // Конструктор: три вершины A, B, C
    triangle(const point3 &A, const point3 &B, const point3 &C, shared_ptr<material> mat)
        : quad(A, B - A, C - A, mat)
    {
        // quad уже вычислил normal, w, bbox и т.д.
        // Нам остаётся только переопределить поведение is_interior
    }

    // Переопределяем is_interior для треугольника
    bool is_interior(double a, double b, hit_record &rec) const override
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

inline shared_ptr<hittable_list> pyramid(const point3 &base_center, double half_width, double height, shared_ptr<material> mat)
{
    auto sides = make_shared<hittable_list>();

    // Четыре угла основания (Y = 0)
    auto hw = half_width;
    auto h = height;

    point3 c = base_center;
    point3 bl(c.x() - hw, c.y(), c.z() - hw); // back-left
    point3 br(c.x() + hw, c.y(), c.z() - hw); // back-right
    point3 fr(c.x() + hw, c.y(), c.z() + hw); // front-right
    point3 fl(c.x() - hw, c.y(), c.z() + hw); // front-left
    point3 top(c.x(), c.y() + h, c.z());      // вершина

    // Основание (квадрат, направлено вниз)
    sides->add(make_shared<quad>(bl, br - bl, fl - bl, mat));

    // Боковые грани — треугольники (если у вас есть triangle)
    sides->add(make_shared<triangle>(bl, br, top, mat));
    sides->add(make_shared<triangle>(br, fr, top, mat));
    sides->add(make_shared<triangle>(fr, fl, top, mat));
    sides->add(make_shared<triangle>(fl, bl, top, mat));

    return sides;
}

// Создаёт пирамиду с произвольным четырёхугольным основанием и вершиной
inline shared_ptr<hittable_list> irregular_pyramid(
    const point3 &v0,   // вершина 0 основания
    const point3 &v1,   // вершина 1 основания
    const point3 &v2,   // вершина 2 основания
    const point3 &v3,   // вершина 3 основания
    const point3 &apex, // вершина пирамиды
    shared_ptr<material> mat)
{
    auto sides = make_shared<hittable_list>();

    // Основание — quad (может быть трапецией, ромбом, любым четырёхугольником)
    // quad строится из точки v0 и векторов к v1 и v3
    sides->add(make_shared<quad>(v0, v1 - v0, v3 - v0, mat));

    // Боковые грани — треугольники
    sides->add(make_shared<triangle>(v0, v1, apex, mat));
    sides->add(make_shared<triangle>(v1, v2, apex, mat));
    sides->add(make_shared<triangle>(v2, v3, apex, mat));
    sides->add(make_shared<triangle>(v3, v0, apex, mat));

    return sides;
}

// Создаёт пирамиду с произвольным четырёхугольным основанием и вершиной
inline shared_ptr<hittable_list> triangle_pyramid(
    const point3 &v0,   // вершина 0 основания
    const point3 &v1,   // вершина 1 основания
    const point3 &v2,   // вершина 2 основания
    const point3 &apex, // вершина пирамиды
    shared_ptr<material> mat)
{
    auto sides = make_shared<hittable_list>();

    // Основание — quad (может быть трапецией, ромбом, любым четырёхугольником)
    // quad строится из точки v0 и векторов к v1 и v3
    // ⚠️ Важно: основание должно быть планарным и выпуклым!
    sides->add(make_shared<triangle>(v0, v1, v2, mat));

    // Боковые грани — треугольники
    sides->add(make_shared<triangle>(v0, v1, apex, mat));
    sides->add(make_shared<triangle>(v1, v2, apex, mat));
    sides->add(make_shared<triangle>(v2, v0, apex, mat));

    return sides;
}

#endif