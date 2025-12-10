#pragma once

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
    int samples_per_pixel = 100; // Количество лучей на пиксель
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

    double pixel_samples_scale; // Скейл фактор для суммы цветов лучей
    Point3 center;              // Центр камеры
    Point3 pixel00_loc;         // Локация пикселя 0, 0
    Vec3 pixel_delta_u;         // Смещение пикселя вправо
    Vec3 pixel_delta_v;         // Смещение пикселя вниз
    Vec3 u, v, w;               // Базисные векторы
    Vec3 defocus_disk_u;        // Горизонтальный радиус диска расфокусировки
    Vec3 defocus_disk_v;        // Вертикальный радиус диска расфокусировки
    Camera _camera;             // Активная камера

    void initialize()
    {
        image_height = _size.x();
        image_width = _size.y();
        pixel_samples_scale = 1.0 / samples_per_pixel;

        center = _camera.lookfrom;

        // Определяем размер viewport
        auto theta = degrees_to_radians(_camera.vfov);
        auto h = std::tan(theta / 2); // Высота вычисляется через тангенс половины угла обзора:
        auto viewport_height = 2 * h * _camera.focus_dist;
        auto viewport_width = viewport_height * (double(image_width) / image_height); // Умножем высоту на соотношение сторон

        // Постоение ортогонального базиса камеры
        w = unit_vector(_camera.lookfrom - _camera.lookat);
        u = unit_vector(cross(_camera.vup, w)); // горизонтальный вектор перпендекулярный w и vup
        v = cross(w, u);                        // вектор перепендекулярный w и u

        // Векторы, описывающие полный размер viewport в мировых координатах
        Vec3 viewport_u = viewport_width * u;   // Горизонтальный вектор — ширина viewport
        Vec3 viewport_v = viewport_height * -v; // Вертикальный вектор — высота viewport (вниз)

        // Сдвиги в каждом направлении
        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // Вычисление лоакации в верхнем левом
        // center - focus_dist * w точка на линии взгляда на расстоянии focus_dist от камеры. Это центр плоскости проекции.
        auto viewport_upper_left = center - (_camera.focus_dist * w) - viewport_u / 2 - viewport_v / 2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v); // Позиция центра первого (0,0) пикселя

        // Вычислени базисных векторов круга расфокусировки
        // _camera.defocus_angle — угол расфокусировки. Делим его на 2, потому что угол измеряется от центральной оси до края круга.
        // Преобразуем в радианы Умножаем на _camera.focus_dist — расстояние до плоскости фокусировк
        auto defocus_radius = _camera.focus_dist * std::tan(degrees_to_radians(_camera.defocus_angle / 2)); // Радиус круга расфокусировки
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }

    /**
     * @brief генерация одного луча трассировки
     */
    Ray get_ray(int i, int j) const
    {
        // offset для Monte Carlo sampling. для антиальяскинга
        auto offset = sample_square();
        auto pixel_sample = pixel00_loc + ((i + offset.x()) * pixel_delta_u) + ((j + offset.y()) * pixel_delta_v);

        auto ray_origin = (_camera.defocus_angle <= 0) ? center : defocus_disk_sample();
        auto ray_direction = pixel_sample - ray_origin;

        return Ray(ray_origin, ray_direction);
    }

    /**
     * @brief возвращает рандомную точку в пределах от [-0.5, -0.5] до [0.5, 0.5]
     */
    Vec3 sample_square() const
    {
        return Vec3(random_double() - 0.5, random_double() - 0.5, 0);
    }

    /**
     * @brief Возвращает случайную точку внутри круга радиуса radius, лежащего в плоскости XY и центрированного в начале координат (0,0,0)
     */
    Vec3 sample_disk(double radius) const
    {
        return radius * random_in_unit_disk();
    }

    /// @brief Возвращает точку в дискре размытия
    /// @return
    Point3 defocus_disk_sample() const
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
        // Учёт излучения материала
        Color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);
        // Попытка рассеяния луча
        if (!rec.mat->scatter(r, rec, attenuation, scattered))
            return color_from_emission;

        // Рекурсивный вызов для рассеянного луча
        Color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world, cancel_running);

        // Эмиссия + рассеянный свет
        return color_from_emission + color_from_scatter;
    }
};
