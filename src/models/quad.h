#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"
#include "hittable_list.h"

class Quad : public Hittable
{
public:
    Quad(const Point3 &Q, const Vec3 &u, const Vec3 &v, shared_ptr<Material> _mat)
        : Q(Q), u(u), v(v)
    {
        mat = _mat;
        auto n = cross(u, v);
        normal = unit_vector(n);
        D = dot(normal, Q);
        w = n / dot(n, n);

        set_bounding_box();
    }

    virtual void set_bounding_box()
    {
        // Compute the bounding box of all four vertices.
        auto bbox_diagonal1 = Aaab(Q, Q + u + v);
        auto bbox_diagonal2 = Aaab(Q + u, Q + v);
        bbox = Aaab(bbox_diagonal1, bbox_diagonal2);
    }

    Aaab bounding_box() const override { return bbox; }

    bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
    {
        if (!is_visible)
            return false;
        auto denom = dot(normal, r.direction());

        // No hit if the ray is parallel to the plane.
        if (std::fabs(denom) < 1e-8)
            return false;

        // Return false if the hit point parameter t is outside the ray interval.
        auto t = (D - dot(normal, r.origin())) / denom;
        if (!ray_t.contains(t))
            return false;

        // Determine if the hit point lies within the planar shape using its plane coordinates.
        auto intersection = r.at(t);
        Vec3 planar_hitpt_vector = intersection - Q;
        auto alpha = dot(w, cross(planar_hitpt_vector, v));
        auto beta = dot(w, cross(u, planar_hitpt_vector));

        if (!is_interior(alpha, beta, rec))
            return false;

        // Ray hits the 2D shape; set the rest of the hit record and return true.
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, HitRecord &rec) const
    {
        Interval unit_interval = Interval(0, 1);
        // Given the hit point in plane coordinates, return false if it is outside the
        // primitive, otherwise set the hit record UV coordinates and return true.

        if (!unit_interval.contains(a) || !unit_interval.contains(b))
            return false;

        rec.u = a;
        rec.v = b;
        return true;
    }

private:
    Point3 Q;
    Vec3 u, v;
    Vec3 w;
    // shared_ptr<Material> mat;
    Aaab bbox;
    Vec3 normal;
    double D;
};

inline shared_ptr<HittableList> box(const Point3 &a, const Point3 &b, shared_ptr<Material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.

    auto sides = make_shared<HittableList>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = Point3(std::fmin(a.x(), b.x()), std::fmin(a.y(), b.y()), std::fmin(a.z(), b.z()));
    auto max = Point3(std::fmax(a.x(), b.x()), std::fmax(a.y(), b.y()), std::fmax(a.z(), b.z()));

    auto dx = Vec3(max.x() - min.x(), 0, 0);
    auto dy = Vec3(0, max.y() - min.y(), 0);
    auto dz = Vec3(0, 0, max.z() - min.z());

    sides->add(make_shared<Quad>(Point3(min.x(), min.y(), max.z()), dx, dy, mat));  // front
    sides->add(make_shared<Quad>(Point3(max.x(), min.y(), max.z()), -dz, dy, mat)); // right
    sides->add(make_shared<Quad>(Point3(max.x(), min.y(), min.z()), -dx, dy, mat)); // back
    sides->add(make_shared<Quad>(Point3(min.x(), min.y(), min.z()), dz, dy, mat));  // left
    sides->add(make_shared<Quad>(Point3(min.x(), max.y(), max.z()), dx, -dz, mat)); // top
    sides->add(make_shared<Quad>(Point3(min.x(), min.y(), min.z()), dx, dz, mat));  // bottom

    return sides;
}

#endif