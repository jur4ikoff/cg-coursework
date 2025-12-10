#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"

class Sphere : public Hittable
{
public:
  Sphere(const Point3 &static_center, double radius, shared_ptr<Material> _mat)
      : center(static_center), radius(std::fmax(0, radius)) // , mat(mat)
  {
    mat = _mat;
    auto rvec = Vec3(radius, radius, radius);
    bbox = Aaab(static_center - rvec, static_center + rvec);
  }

  bool hit(const Ray &r, Interval ray_t, HitRecord &rec) const override
  {
    // Point3 current_center = center.at(r.time());
    // Vec3 oc = current_center - r.origin();
    // Vec3 oc = r.origin();
    Vec3 oc = center - r.origin();
    auto a = r.direction().length_squared();
    auto h = dot(r.direction(), oc);
    auto c = oc.length_squared() - radius * radius;

    if (!is_visible)
      return false;

    auto discriminant = h * h - a * c;
    if (discriminant < 0)
      return false;

    auto sqrtd = std::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range.
    auto root = (h - sqrtd) / a;
    if (!ray_t.surrounds(root))
    {
      root = (h + sqrtd) / a;
      if (!ray_t.surrounds(root))
        return false;
    }

    rec.t = root;
    rec.p = r.at(rec.t);
    // Vec3 outward_normal = (rec.p - current_center) / radius;
    Vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    get_sphere_uv(outward_normal, rec.u, rec.v);
    rec.mat = mat;

    return true;
  }

  Aaab bounding_box() const override { return bbox; }

private:
  Point3 center;
  double radius;
  // shared_ptr<Material> mat;
  Aaab bbox;

  static void get_sphere_uv(const Point3 &p, double &u, double &v)
  {
    // p: a given point on the sphere of radius one, centered at the origin.
    // u: returned value [0,1] of angle around the Y axis from X=-1.
    // v: returned value [0,1] of angle from Y=-1 to Y=+1.
    //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
    //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
    //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

    auto theta = std::acos(-p.y());
    auto phi = std::atan2(-p.z(), p.x()) + M_PI;

    u = phi / (2 * M_PI);
    v = theta / M_PI;
  }
};

#endif
