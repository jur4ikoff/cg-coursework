#include "raytraicer.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "color.h"
#include "vec3.h"
#include "ray.h"

Color sky(Ray r)
{
    // Приведение к единичному вектору
    Vec3 unit_direction = unit_vector(r.direction());
    double a = 0.5 * (unit_direction.y() + 1.0);

    // По формуле линейной интерполяци
    // blended_value = (1 - a) * start_value + a * end_value
    return (1.0 - a) * Color(1.0, 1.0, 1.0) + a * Color(0.5, 0.7, 1.0);
}

double hit_sphere(const Point3 &center, double radius, const Ray &r)
{
    // Решаем квадраное уравнение t^2 * d - 2td * (C - Q) + (C - Q) * (C - Q) - r^2 =0
    // Вектор между началом луча и центром окруност
    Vec3 oc = center - r.origin();

    double a = r.direction().lenght_squared();
    double h = dot(r.direction(), oc);
    double c = oc.lenght_squared() - radius * radius;

    double discriminant = h * h - a * c;

    if (discriminant < 0)
    {
        return -1.0;
    }
    else
    {
        return (h - std::sqrt(discriminant)) / a;
    }
}

Color ray_color(const Ray &ray, const Hittable &world)
{
    HitRecord rec;
    if (world.hit(ray, Interval(0, INF), rec))
    {
        return 0.5 * (rec.normal + Color(1, 1, 1));
    }

    return sky(ray);
}

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

    // World
    HittableList world;
    world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));
    world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));

    // render
    for (int j = 0; j < size.height(); j++)
    {
        for (int i = 0; i < size.width(); i++)
        {
            auto pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v);
            auto ray_direction = pixel_center - camera_center;
            Ray ray(camera_center, ray_direction);

            Color pixel_color = ray_color(ray, world);
            color_matrix.at(j, i) = pixel_color;
        }
    }

    return color_matrix;
}
