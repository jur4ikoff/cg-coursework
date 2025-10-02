#pragma once

#include "hittable.h"
#include "color_matrix.h"
#include "constants.h"
#include "vec3.h"
// #include "color.h"

class Camera
{
public:
    Size _size;
    double aspect_ratio;
    int samples_per_pixel = 10;

    void render(const Hittable &world, ColorMatrix &color_matrix, Size size)
    {
        _size = size;
        initialize();

        // render
        for (int j = 0; j < _size.y(); j++)
        {
            for (int i = 0; i < _size.x(); i++)
            {
                Color pixel_color(0, 0, 0);
                for (int sample = 0; sample < samples_per_pixel; sample++)
                {
                    Ray ray = get_ray(i, j);
                    pixel_color += ray_color(ray, world);
                }
                color_matrix.at(j, i) = pixel_samples_scale * pixel_color;
            }
        }
    }

private:
    double focal_length;
    double pixel_samples_scale;

    MyPoint3 center;

    MyVec3 viewport_u;
    MyVec3 viewport_v;

    MyVec3 pixel_delta_u;
    MyVec3 pixel_delta_v;

    MyPoint3 viewport_upper_left;
    MyPoint3 pixel00_loc;

    void initialize()
    {
        aspect_ratio = static_cast<double>(_size.x()) / _size.y();

        focal_length = 1.0;

        double viewport_height = 2.0;
        double viewport_width = viewport_height * (double(_size.x()) / _size.y());

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = MyPoint3(0, 0, 0);

        viewport_u = MyVec3(viewport_width, 0, 0);
        viewport_v = MyVec3(0, -viewport_height, 0);

        pixel_delta_u = viewport_u / _size.x();
        pixel_delta_v = viewport_v / _size.y();

        viewport_upper_left = center - MyVec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
    }

    Color sky(Ray r) const
    {
        // Приведение к единичному вектору
        MyVec3 unit_direction = unit_vector(r.direction());
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

    Ray get_ray(int i, int j) const
    {
        // MyVec3 offset = sample_square();
        MyVec3 offset(0, 0, 0);

        // std::cout << offset << std::endl;
        // std::cout << pixel_delta_u << std::endl;
        // std::cout << pixel_delta_v << std::endl;
        // std::cout << "_-----___-----------------\n";
        // MyVec3 pixel_sample = pixel00_loc + ((i + offset.x() * pixel_delta_u) + (j + offset.y() * pixel_delta_v));
        // MyVec3 pixel_sample = pixel00_loc + ((i * pixel_delta_u) + (j * pixel_delta_v));
        // MyVec3 pixel_sample = pixel00_loc + ((i * (offset.x() + pixel_delta_u)) + (j * (offset.y() + pixel_delta_v)));
        MyVec3 pixel_sample = pixel00_loc + ((i * (offset.x() + pixel_delta_u)) + (j * (offset.y() + pixel_delta_v)));

        MyPoint3 ray_origin = center;
        MyVec3 ray_direction = pixel_sample - ray_origin;
        return Ray(ray_origin, ray_direction);
    }

    MyVec3 sample_square() const
    {
        // return MyVec3(random_double_2(-0.1, 0,1) - 0.5, random_double_2() - 0.5, 0);
        return MyVec3(random_double(-0.0001, 0.0001), random_double(-0.0001, 0.0001), 0);
    }
};