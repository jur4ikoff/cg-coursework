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
    Point3 center;              // Центр камеры
    Point3 pixel00_loc;         // Локация пикселя 0, 0
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

    Point3 defocus_disk_sample() const
    {
        // Returns a random point in the Render defocus disk.
        auto p = random_in_unit_disk();
        return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    // Color ray_color(const Ray &r, int depth, const Hittable &world, volatile bool &cancel_running) const
    // {
    //     if (cancel_running)
    //         return background;

    //     // Если превысили лимит отражений, то выходим
    //     if (depth <= 0)
    //         return Color(0, 0, 0);

    //     HitRecord rec;

    //     // Если луч ничего не пересекает, то возвращаем цвет пикселя
    //     if (!world.hit(r, Interval(0.001, infinity), rec))
    //         return background;

    //     Ray scattered;
    //     Color attenuation;
    //     Color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

    //     if (!rec.mat->scatter(r, rec, attenuation, scattered))
    //         return color_from_emission;

    //     Color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world, cancel_running);

    //     return color_from_emission + color_from_scatter;
    // }
    Color ray_color(const Ray &r, int depth, const Hittable &world, volatile bool &cancel_running) const
    {
        if (cancel_running)
            return background;

        if (depth <= 0)
            return Color(0, 0, 0);

        HitRecord rec;
        const double fog_density = 0.005;               // настраиваемый параметр
        const Color fog_color = Color(0, 0, 0); // цвет тумана (например, голубоватый)

        if (!world.hit(r, Interval(0.001, infinity), rec))
        {
            // Луч уходит в бесконечность → применяем туман к фону
            // В простом случае можно вернуть fog_color, но лучше — затухание фона
            return fog_color; // или: background * exp(-fog_density * some_max_dist)
        }

        // Расстояние до объекта
        double distance = (rec.p - r.origin()).length();
        double fog_factor = exp(-fog_density * distance);

        // Рекурсивный вызов (без тумана внутри объекта)
        Ray scattered;
        Color attenuation;
        Color emitted = rec.mat->emitted(rec.u, rec.v, rec.p);

        if (!rec.mat->scatter(r, rec, attenuation, scattered))
        {
            // Нет рассеяния → просто затухший эмитированный цвет
            return emitted * fog_factor + fog_color * (1 - fog_factor);
        }

        Color scattered_color = attenuation * ray_color(scattered, depth - 1, world, cancel_running);
        Color object_color = emitted + scattered_color;

        // Смешиваем цвет объекта с туманом
        return object_color * fog_factor + fog_color * (1 - fog_factor);
    }

    // Color ray_color_with_exp_fog(const Ray &r, int depth, const Hittable &world, volatile bool &cancel_running) const
    // {
    //     if (cancel_running)
    //         return background;

    //     if (depth <= 0)
    //         return Color(0, 0, 0);

    //     HitRecord rec;
    //     if (!world.hit(r, Interval(0.001, infinity), rec))
    //     {
    //         // Нет пересечения — просто возвращаем фон, но можно тоже затуманить,
    //         // если хотите "бесконечный" туман (необязательно)
    //         return background;
    //     }

    //     // === ПАРАМЕТРЫ ТУМАНА ===
    //     const double fog_density = 0.002;         // Настройте: 0.01–0.5 (чем больше — гуще)
    //     const Color fog_color = Color(0, 0, 0); // Цвет тумана: белый, можно Color(0.8, 0.8, 0.9)

    //     // Расстояние от начала луча до точки пересечения
    //     double ray_dist = rec.t * r.direction().length();

    //     // Пропускание (transmittance) по экспоненциальному закону
    //     double transmittance = std::exp(-fog_density * ray_dist);
    //     // Ограничиваем значение, чтобы избежать артефактов при больших расстояниях
    //     transmittance = std::clamp(transmittance, 0.0, 1.0);

    //     // Получаем цвет от эмиссии (он тоже должен затухать в тумане!)
    //     Color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

    //     Ray scattered;
    //     Color attenuation;
    //     if (!rec.mat->scatter(r, rec, attenuation, scattered))
    //     {
    //         // Нет рассеяния — только эмиссия, затухающая в тумане
    //         return transmittance * color_from_emission + (1.0 - transmittance) * fog_color;
    //     }

    //     // Рекурсивный вызов для рассеянного луча
    //     Color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world, cancel_running);

    //     // Общий цвет объекта (до применения тумана)
    //     Color object_color = color_from_emission + color_from_scatter;

    //     // Применяем туман: смешиваем цвет объекта с цветом тумана
    //     return transmittance * object_color + (1.0 - transmittance) * fog_color;
    // }

    // Color ray_color(const Ray &r, int depth, const Hittable &world, volatile bool &cancel_running) const
    // {
    //     Perlin perlin_noise = Perlin();
    //     if (cancel_running)
    //         return background;

    //     if (depth <= 0)
    //         return Color(0, 0, 0);

    //     HitRecord rec;
    //     if (!world.hit(r, Interval(0.001, infinity), rec))
    //     {
    //         // Фон: можно либо вернуть background, либо затуманить "в бесконечность"
    //         // Здесь — просто фон (или замените на затуманенный, как ниже)
    //         return background;
    //     }

    //     // === ПАРАМЕТРЫ НЕОДНОРОДНОГО ТУМАНА ===
    //     const double base_density = 0.001;               // Базовая плотность (0.05–0.3)
    //     const double noise_scale = 0.02;               // Масштаб шума (меньше → крупнее облака)
    //     const double noise_strength = 0.4;             // Насколько сильно шум модулирует плотность [0,1]
    //     const Color fog_color = Color(0, 0, 0); // Холодный бело-голубой туман

    //     // Направление и длина луча (предполагается, что r.direction() не нормирован)
    //     Vec3 ray_dir = r.direction();
    //     double ray_len = ray_dir.length();
    //     Vec3 unit_dir = ray_dir / ray_len;

    //     double t0 = 0.001;
    //     double t1 = rec.t;

    //     // === ЧИСЛЕННОЕ ИНТЕГРИРОВАНИЕ ПЛОТНОСТИ ВДОЛЬ ЛУЧА ===
    //     const int STEPS = 32; // Качество/производительность: 32–128
    //     double transmittance = 1.0;

    //     // Используем метод прямоугольников (можно улучшить до трапеций)
    //     for (int i = 0; i < STEPS; ++i)
    //     {
    //         double t = t0 + (t1 - t0) * (i + 0.5) / STEPS;
    //         Point3 p = r.at(t);

    //         // Плотность в точке: база + шум
    //         double noise_val = perlin_noise.noise(noise_scale * p);
    //         // Сдвигаем в [0,1] и усиливаем контраст по желанию
    //         noise_val = 0.5 + 0.5 * noise_val; // теперь [0,1]
    //         // Или: noise_val = std::fabs(noise_val); // для "пушистых" облаков

    //         double local_density = base_density * (1.0 + noise_strength * noise_val);
    //         // Альтернатива: local_density = base_reserved_density * std::pow(noise_val, 3); // более "клубисто"

    //         // Вклад в оптическую глубину
    //         double segment_length = (t1 - t0) * ray_len / STEPS;
    //         transmittance *= std::exp(-local_density * segment_length);
    //     }

    //     transmittance = std::clamp(transmittance, 0.0, 1.0);

    //     // Цвет объекта (эмиссия + рассеяние)
    //     Color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

    //     Ray scattered;
    //     Color attenuation;
    //     if (!rec.mat->scatter(r, rec, attenuation, scattered))
    //     {
    //         Color object_color = color_from_emission;
    //         return transmittance * object_color + (1.0 - transmittance) * fog_color;
    //     }

    //     Color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world, cancel_running);
    //     Color object_color = color_from_emission + color_from_scatter;

    //     return transmittance * object_color + (1.0 - transmittance) * fog_color;
    // }
};

#endif // RENDER_H
