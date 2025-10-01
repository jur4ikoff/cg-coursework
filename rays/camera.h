#pragma once

#include "hittable.h"
#include "color_matrix.h"
#include "color.h"

class Camera
{
public:
    Size _size;
    double aspect_ratio;

    void render(const Hittable &world, ColorMatrix &color_matrix, Size size)
    {
        _size = size;
        initialize();

        // render
        for (int j = 0; j < _size.y(); j++)
        {
            for (int i = 0; i < _size.x(); i++)
            {
                auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
                auto ray_direction = pixel_center - center;
                Ray ray(center, ray_direction);

                Color pixel_color = ray_color(ray, world);
                color_matrix.at(j, i) = pixel_color;
            }
        }
    }

private:
    double focal_length;

    Point3 center;

    Vec3 viewport_u;
    Vec3 viewport_v;

    Vec3 pixel_delta_u;
    Vec3 pixel_delta_v;

    Point3 viewport_upper_left;
    Point3 pixel00_loc;

    void initialize()
    {
        aspect_ratio = static_cast<double>(_size.x()) / _size.y();

        focal_length = 1.0;

        double viewport_height = 2.0;
        double viewport_width = viewport_height * (double(_size.x()) / _size.y());

        center = Point3(0, 0, 0);

        viewport_u = Vec3(viewport_width, 0, 0);
        viewport_v = Vec3(0, -viewport_height, 0);

        pixel_delta_u = viewport_u / _size.x();
        pixel_delta_v = viewport_v / _size.y();

        viewport_upper_left = center - Vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    Color sky(Ray r) const
    {
        // Приведение к единичному вектору
        Vec3 unit_direction = unit_vector(r.direction());
        double a = 0.5 * (unit_direction.y() + 1.0);

        // По формуле линейной интерполяци
        // blended_value = (1 - a) * start_value + a * end_value
        return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
    }

    Color ray_color(const Ray &ray, const Hittable &world) const
    {
        HitRecord rec;
        if (world.hit(ray, Interval(0, INF), rec))
        {
            return 0.5 * (rec.normal + Color(1, 1, 1));
        }

        return sky(ray);
    }
};