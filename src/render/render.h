#ifndef RENDER_H
#define RENDER_H

#include "hittable.h"
#include "material.h"
#include "color_matrix.h"
#include "task.h"
#include "camera.h"

class RenderSettings
{
public:
    RenderSettings() = default;
    RenderSettings(int samples, int depth, Color back) : samples_per_pixel(samples),
                                                         max_depth(depth),
                                                         background(back) {}

    int samples_per_pixel = 100;
    int max_depth = 10;
    Color background{0.5, 0.5, 0.5};
};

class Render
{
    friend class RenderTask;

public:
    int samples_per_pixel = 100; // Количество лучей на каждый пиксель
    int max_depth = 10;          // Максимальное количество переотражений
    Color background;            // Фоновый цвет

    void render(const Hittable &world, ColorMatrix &color_matrix,
                volatile bool &cancel_running, size_t thread_count,
                std::function<void()> tile_callback);

    void set_camera(Camera &camera)
    {
        _camera = camera;
    }

    void set_render_settings(RenderSettings &settings);
    Camera &get_camera()
    {
        return _camera;
    }

private:
    int image_width = 600;
    int image_height = 600;
    Size _size;

    double pixel_samples_scale; // Color scale factor for a sum of pixel samples
    point3 center;              // Центр камеры
    point3 pixel00_loc;         // Локация пикселя 0, 0
    Vec3 pixel_delta_u;         // Смещение пикселя вправо
    Vec3 pixel_delta_v;         // Смещение пикселя вниз
    Vec3 u, v, w;               // Базисные векторы
    Vec3 defocus_disk_u;        // Горизонтальный радиус диска расфокусировки
    Vec3 defocus_disk_v;        // Вертикальный радиус диска расфокусировки
    Camera _camera;

    void initialize()
    {
        image_height = _size.x();
        image_width = _size.y();

        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = _camera.lookfrom;

        // Determine viewport dimensions.
        auto theta = degrees_to_radians(_camera.vfov);
        auto h = std::tan(theta / 2);
        auto viewport_height = 2 * h * _camera.focus_dist;
        auto viewport_width = viewport_height * (double(image_width) / image_height);

        // Calculate the u,v,w unit basis vectors for the Render coordinate frame.
        w = unit_vector(_camera.lookfrom - _camera.lookat);
        u = unit_vector(cross(_camera.vup, w));
        v = cross(w, u);

        // Calculate the vectors across the horizontal and down the vertical viewport edges.
        Vec3 viewport_u = viewport_width * u;   // Vector across viewport horizontal edge
        Vec3 viewport_v = viewport_height * -v; // Vector down viewport vertical edge

        // Calculate the horizontal and vertical delta vectors from pixel to pixel.
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Calculate the location of the upper left pixel.
        auto viewport_upper_left = center - (_camera.focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        // Calculate the Render defocus disk basis vectors.
        auto defocus_radius = _camera.focus_dist * std::tan(degrees_to_radians(_camera.defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    Ray get_ray(int i, int j) const
    {
        // Construct a Render ray originating from the defocus disk and directed at a randomly
        // sampled point around the pixel location i, j.

        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (_camera.defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;
        auto ray_time = random_double();

        return Ray(ray_origin, ray_direction, ray_time);
    }

    Vec3 sample_square() const
    {
        // Returns the vector to a random point in the [-.5,-.5]-[+.5,+.5] unit square.
        return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    Vec3 sample_disk(double radius) const
    {
        // Returns a random point in the unit (radius 0.5) disk centered at the origin.
        return radius * random_in_unit_disk();
    }

    point3 defocus_disk_sample() const
    {
        // Returns a random point in the Render defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    Color ray_color(const Ray &r, int depth, const Hittable &world, volatile bool &cancel_running) const
    {
        if (cancel_running)
            return background;

        // Если превысили лимит отражений, то выходим
        if (depth <= 0)
            return Color(0, 0, 0);

        HitRecord rec;

        // Если луч ничего не пересекает, то возвращаем цвет пикселя
        if (!world.hit(r, Interval(0.001, infinity), rec))
            return background;

        Ray scattered;
        Color attenuation;
        Color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        Color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world, cancel_running);

        return color_from_emission + color_from_scatter;
    }
};

#endif // RENDER_H
