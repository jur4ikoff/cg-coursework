#include "scene.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "camera.h"
#include "material.h"

// Color ray_color(const Ray &ray, const Hittable &world)
// {
//     HitRecord rec;
//     if (world.hit(ray, Interval(0, INF), rec))
//     {
//         return 0.5 * (rec.normal + Color(1, 1, 1));
//     }

//     return sky(ray);
// }

ColorMatrix Scene::draw(QSize size)
{
    ColorMatrix color_matrix(size.height(), size.width());
    HittableList world;

    auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

    for (int a = -8; a < 8; a++)
    {
        for (int b = -8; b < 8; b++)
        {
            auto choose_mat = random_double();
            Point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            if ((center - Point3(4, 0.2, 0)).length() > 0.9)
            {
                shared_ptr<Material> sphere_material;

                if (choose_mat < 0.8)
                {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    sphere_material = make_shared<Lambertian>(albedo);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else if (choose_mat < 0.95)
                {
                    // metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                else
                {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    Camera camera;
    camera.samples_per_pixel = 10;
    camera.max_depth = 12;

    camera.vfov = 30;
    camera.lookfrom = Point3(13, 2, 3);
    camera.lookat = Point3(0, 0, 0);
    camera.vup = Vec3(0, 1, 0);

    camera.defocus_angle = 0.2;
    camera.focus_dist = 10.0;

    camera.render(world, color_matrix, Size(size.width(), size.height(), 0));

    return color_matrix;
}
