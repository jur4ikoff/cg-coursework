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

    std::shared_ptr<Lambertian> material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    std::shared_ptr<Lambertian> material_center = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    std::shared_ptr<Metal> material_left = make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.3);
    std::shared_ptr<Metal> material_right = make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    HittableList world;
    world.add(make_shared<Sphere>(Point3(0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<Sphere>(Point3(0.0, 0.0, -1.2), 0.5, material_center));
    world.add(make_shared<Sphere>(Point3(-1.0, 0.0, -1.0), 0.5, material_left));
    world.add(make_shared<Sphere>(Point3(1.0, 0.0, -1.0), 0.5, material_right));

    Camera camera;
    camera.samples_per_pixel = 20;
    camera.max_depth = 20;
    camera.render(world, color_matrix, Size(size.width(), size.height(), 0));
    return color_matrix;
}
