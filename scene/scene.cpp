#include "scene.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "color.h"
#include "vec3.h"
#include "ray.h"
#include "camera.h"

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

    // World
    HittableList world;
    world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));
    world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));

    Camera camera;
    camera.render(world, color_matrix, Size(size.width(), size.height(), 0));
    return color_matrix;
}
