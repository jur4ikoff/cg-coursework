#include "raytraicer.h"
#include "color.h"
#include "vec3.h"
#include "ray.h"

ColorMatrix RayTraicer::draw(QSize size)
{
    ColorMatrix color_matrix(size.height(), size.width());

    // потом проверить определение размеров на всякий случай
    double aspect_ratio = static_cast<double>(size.width()) / size.height();

    double viewport_height = 2.0;
    double viewport_width = viewport_height * (double(size.width()) / size.height()); // Можно заменить на aspect_ratio
    double focal_length = 1.0;
    Point3 camera_center(0, 0, 0);

    Vec3 viewport_u(viewport_width, 0, 0);
    Vec3 viewport_v(0, -viewport_height, 0);

    Vec3 pixel_delta_u = viewport_u / size.width();
    Vec3 pixel_delta_v = viewport_v / size.height();

    Point3 viewport_upper_left = camera_center - Vec3(0, 0, focal_length) - viewport_u / 2 - viewport_v / 2;
    Point3 pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

    // render

    for (int j = 0; j < size.height(); j++)
    {
        for (int i = 0; i < size.width(); i++)
        {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            Ray r(camera_center, ray_direction);

            Color pixel_color = ray_color(r);
            color_matrix.at(j, i) = pixel_color;
        }
    }

    return color_matrix;
}