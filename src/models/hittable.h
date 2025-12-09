#ifndef HITTABLE_H
#define HITTABLE_H

#include "aabb.h"
#include "ray.h"

class Material;

class HitRecord
{
public:
    Point3 p;
    Vec3 normal;
    shared_ptr<Material> mat;
    double t;
    double u;
    double v;
    bool front_face;

    /**
     * @brief Устанавливает вектор нормали в запись о попадании
     * @note Параметр outward_normal должен иметь единичную длину
     */
    void set_face_normal(const Ray &r, const Vec3 &outward_normal)
    {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable
{
public:
    virtual ~Hittable() = default;

    virtual bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const = 0;

    virtual Aaab bounding_box() const = 0;

    size_t id = 0;
    shared_ptr<Material> mat;
    bool is_visible = true;
};

class Shift : public Hittable
{
public:
    Shift(shared_ptr<Hittable> object, const Vec3 &offset)
        : object(object), offset(offset)
    {
        bbox = object->bounding_box() + offset;
    }

    bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
    {
        // Move the ray backwards by the offset
        Ray offset_r(r.origin() - offset, r.direction(), r.time());

        // Determine whether an intersection exists along the offset ray (and if so, where)
        if (!object->hit(offset_r, ray_t, rec))
            return false;

        // Move the intersection point forwards by the offset
        rec.p += offset;

        return true;
    }

    Aaab bounding_box() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    Vec3 offset;
    Aaab bbox;
};

class RotateY : public Hittable
{
public:
    RotateY(shared_ptr<Hittable> object, double angle) : object(object)
    {
        auto radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        Point3 min(infinity, infinity, infinity);
        Point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    auto newx = cos_theta * x + sin_theta * z;
                    auto newz = -sin_theta * x + cos_theta * z;

                    Vec3 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++)
                    {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = Aaab(min, max);
    }

    bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
    {

        // Transform the ray from world space to object space.

        auto origin = Point3(
            (cos_theta * r.origin().x()) - (sin_theta * r.origin().z()),
            r.origin().y(),
            (sin_theta * r.origin().x()) + (cos_theta * r.origin().z()));

        auto direction = Vec3(
            (cos_theta * r.direction().x()) - (sin_theta * r.direction().z()),
            r.direction().y(),
            (sin_theta * r.direction().x()) + (cos_theta * r.direction().z()));

        Ray rotated_r(origin, direction, r.time());

        // Determine whether an intersection exists in object space (and if so, where).

        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        // Transform the intersection from object space back to world space.

        rec.p = Point3(
            (cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
            rec.p.y(),
            (-sin_theta * rec.p.x()) + (cos_theta * rec.p.z()));

        rec.normal = Vec3(
            (cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
            rec.normal.y(),
            (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z()));

        return true;
    }

    Aaab bounding_box() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    double sin_theta;
    double cos_theta;
    Aaab bbox;
};

class RotateZ : public Hittable
{
public:
    RotateZ(shared_ptr<Hittable> object, double angle) : object(object)
    {
        auto radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        Point3 min(infinity, infinity, infinity);
        Point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    // Поворот вокруг оси Z: (x, y) → (x', y')
                    auto newx = cos_theta * x - sin_theta * y;
                    auto newy = sin_theta * x + cos_theta * y;

                    Vec3 tester(newx, newy, z);

                    for (int c = 0; c < 3; c++)
                    {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = Aaab(min, max);
    }

    bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
    {
        // Преобразуем луч из мировых координат в локальные (обратный поворот вокруг Z)
        auto origin = Point3(
            (cos_theta * r.origin().x()) + (sin_theta * r.origin().y()),
            (-sin_theta * r.origin().x()) + (cos_theta * r.origin().y()),
            r.origin().z());

        auto direction = Vec3(
            (cos_theta * r.direction().x()) + (sin_theta * r.direction().y()),
            (-sin_theta * r.direction().x()) + (cos_theta * r.direction().y()),
            r.direction().z());

        Ray rotated_r(origin, direction, r.time());

        // Проверяем пересечение в локальной системе
        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        // Преобразуем точку и нормаль обратно в мировые координаты (прямой поворот)
        rec.p = Point3(
            (cos_theta * rec.p.x()) - (sin_theta * rec.p.y()),
            (sin_theta * rec.p.x()) + (cos_theta * rec.p.y()),
            rec.p.z());

        rec.normal = Vec3(
            (cos_theta * rec.normal.x()) - (sin_theta * rec.normal.y()),
            (sin_theta * rec.normal.x()) + (cos_theta * rec.normal.y()),
            rec.normal.z());

        return true;
    }

    Aaab bounding_box() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    double sin_theta;
    double cos_theta;
    Aaab bbox;
};

class RotateX : public Hittable
{
public:
    RotateX(shared_ptr<Hittable> object, double angle) : object(object)
    {
        auto radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        Point3 min(infinity, infinity, infinity);
        Point3 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    auto x = i * bbox.x.max + (1 - i) * bbox.x.min;
                    auto y = j * bbox.y.max + (1 - j) * bbox.y.min;
                    auto z = k * bbox.z.max + (1 - k) * bbox.z.min;

                    // Поворот вокруг оси X: (y, z) → (y', z')
                    auto newy = cos_theta * y - sin_theta * z;
                    auto newz = sin_theta * y + cos_theta * z;

                    Vec3 tester(x, newy, newz);

                    for (int c = 0; c < 3; c++)
                    {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = Aaab(min, max);
    }

    bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
    {
        // Преобразуем луч из мировых координат в локальные (обратный поворот вокруг X)
        auto origin = Point3(
            r.origin().x(),
            (cos_theta * r.origin().y()) + (sin_theta * r.origin().z()),
            (-sin_theta * r.origin().y()) + (cos_theta * r.origin().z()));

        auto direction = Vec3(
            r.direction().x(),
            (cos_theta * r.direction().y()) + (sin_theta * r.direction().z()),
            (-sin_theta * r.direction().y()) + (cos_theta * r.direction().z()));

        Ray rotated_r(origin, direction, r.time());

        // Проверяем пересечение в локальной системе
        if (!object->hit(rotated_r, ray_t, rec))
            return false;

        // Преобразуем точку и нормаль обратно в мировые координаты (прямой поворот)
        rec.p = Point3(
            rec.p.x(),
            (cos_theta * rec.p.y()) - (sin_theta * rec.p.z()),
            (sin_theta * rec.p.y()) + (cos_theta * rec.p.z()));

        rec.normal = Vec3(
            rec.normal.x(),
            (cos_theta * rec.normal.y()) - (sin_theta * rec.normal.z()),
            (sin_theta * rec.normal.y()) + (cos_theta * rec.normal.z()));

        return true;
    }

    Aaab bounding_box() const override { return bbox; }

private:
    shared_ptr<Hittable> object;
    double sin_theta;
    double cos_theta;
    Aaab bbox;
};

#endif