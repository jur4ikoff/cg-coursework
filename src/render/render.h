#pragma once

#include "color_matrix.h"
#include "constants.h"
#include "drawer.h"
#include "hittable.h"
#include "material.h"
#include "vec3.h"

#include <chrono>
#include <memory>
#include <mutex>

class RenderTask;

class Render {
  friend class RenderTask;

public:
  Render() {}
  Size _size;
  double aspect_ratio;
  int samples_per_pixel = 10;
  int max_depth = 10;
  double vfov = 90;

  Point3 lookfrom = Point3(0, 0, 0);
  Point3 lookat = Point3(0, 0, -1);
  Vec3 vup = Vec3(0, 1, 0);

  double defocus_angle = 0; // Variation angle of rays through each pixel
  double focus_dist = 10;   // Distance from camera lookfrom point

  void render(const Hittable &world, ColorMatrix &color_matrix,
              volatile bool &cancel_running, size_t thread_count,
              std::function<void()> tile_callback = nullptr);

  void render_seq(const Hittable &world, ColorMatrix &color_matrix);

private:
  double focal_length;
  double pixel_samples_scale;

  Point3 center;

  Vec3 pixel_delta_u;
  Vec3 pixel_delta_v;

  Point3 viewport_upper_left;
  Point3 pixel00_loc;

  Vec3 u, v, w; // Базисные векторы

  Vec3 defocus_disk_u; // Defocus disk horizontal radius
  Vec3 defocus_disk_v; // Defocus disk vertical radius

  void initialize() {
    aspect_ratio = static_cast<double>(_size.x()) / _size.y();

    pixel_samples_scale = 1.0 / samples_per_pixel;
    center = lookfrom;

    // focal_length = (lookfrom - lookat).length();
    double theta = degrees_to_radians(vfov);
    double h = std::tan(theta / 2);
    auto viewport_height = 2 * h * focus_dist;
    double viewport_width = viewport_height * (double(_size.x()) / _size.y());

    w = unit_vector(lookfrom - lookat);
    u = unit_vector(cross(vup, w));
    v = cross(w, u);

    Vec3 viewport_u =
        viewport_width * u; // Vector across viewport horizontal edge
    Vec3 viewport_v =
        viewport_height * -v; // Vector down viewport vertical edge

    pixel_delta_u = viewport_u / _size.x();
    pixel_delta_v = viewport_v / _size.y();

    viewport_upper_left =
        center - (focus_dist * w) - viewport_u / 2 - viewport_v / 2;
    pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    auto defocus_radius =
        focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
    defocus_disk_u = u * defocus_radius;
    defocus_disk_v = v * defocus_radius;
  }

  Color sky(Ray r) const {
    // Приведение к единичному вектору
    Vec3 unit_direction = unit_vector(r.direction());
    double a = 0.5 * (unit_direction.y() + 1.0);

    // По формуле линейной интерполяци
    // blended_value = (1 - a) * start_value + a * end_value
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
  }

  Color ray_color(const Ray &ray, int depth, const Hittable &world) const {
    if (depth <= 0) {
      return Color(0, 0, 0);
    }

    HitRecord rec;
    if (world.hit(ray, Interval(0.000001, INF), rec)) {
      Ray scattered;
      Color attenuation;
      if (rec.material->scatter(ray, rec, attenuation, scattered)) {
        return attenuation * ray_color(scattered, depth - 1, world);
      }

      return Color(0, 0, 0);
    }

    return sky(ray);
  }

  Ray get_ray(int i, int j) const {
    Vec3 offset = sample_square();

    Vec3 pixel_sample = pixel00_loc + ((i * (offset.x() + pixel_delta_u)) +
                                       (j * (offset.y() + pixel_delta_v)));

    // Point3 ray_origin = center;
    Point3 ray_origin = (defocus_angle <= 0) ? center : defocus_disk_sample();
    Vec3 ray_direction = pixel_sample - ray_origin;
    return Ray(ray_origin, ray_direction);
  }

  Point3 defocus_disk_sample() const {
    // Returns a random point in the camera defocus disk.
    Point3 p = random_in_unit_disk();
    return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
  }

  Vec3 sample_square() const {
    return Vec3(random_double(-0.000002, 0.000002),
                random_double(-0.000002, 0.000002), 0);
  }
};
