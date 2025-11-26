#include "scene.h"

#include "hittable_list.h"
#include "render.h"
#include "sphere.h"
#include "quad.h"
#include "pyramid.h"
#include "cylinder.h"
#include "cone.h"

#include "constant_medium.h"

hittable_list Scene::draw()
{
  hittable_list world;

  auto red = make_shared<lambertian>(color(.65, .05, .05));
  auto white = make_shared<lambertian>(color(.73, .73, .73));
  auto green = make_shared<lambertian>(color(.12, .45, .15));
  auto light = make_shared<diffuse_light>(color(5, 5, 5));
  auto cylinder_mat = make_shared<lambertian>(color(1, 0, 0));
  auto glass = make_shared<dielectric>(1.5);
  auto metal_mat = make_shared<metal>(color(0.8, 0.8, 0.8), 1);

  auto con = make_shared<cone>(point3(350, 150, 350), 100, 200, metal_mat);
  // auto con_r = make_shared<rotate_x>(con, 20);
  world.add(con);

  // auto pyr = pyramid(point3(400, 0, 200), 150, 300, red);
  // auto pyr_r = make_shared<rotate_y>(pyr, -45);
  // world.add(pyr_r);

  // auto pyr = irregular_pyramid(point3(0,0,0), point3(0, 0, 200), point3(200, 0, 200), point3(200, 0, 0), point3(200, 400, 100), metal_mat);
  // world.add(pyr);

  // auto pyr = triangle_pyramid(point3(0, 0, 0), point3(0, 0, 200), point3(200, 0, 200), point3(100, 200, 100), metal_mat);
  // world.add(pyr);

  // auto cylinder_1 = make_shared<cylinder>(50, 0, 100, cylinder_mat);

  // auto tr = make_shared<triangle>(point3(0, 0, 0), point3(100, 400, 100), point3(200, 0, 200), white);
  // auto tr_r = make_shared<rotate_y>(tr, 90);
  // world.add(make_shared<translate>(tr_r, vec3(0, 0, 400)));

  // Cornell box sides
  world.add(make_shared<quad>(point3(555, 0, 0), vec3(0, 0, 555), vec3(0, 555, 0), green));
  world.add(make_shared<quad>(point3(0, 0, 555), vec3(0, 0, -555), vec3(0, 555, 0), red));
  // world.add(make_shared<quad>(point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
  world.add(make_shared<quad>(point3(0, 0, 555), vec3(555, 0, 0), vec3(0, 0, -555), white));
  world.add(make_shared<quad>(point3(555, 0, 555), vec3(-555, 0, 0), vec3(0, 555, 0), white));
  // world.add(make_shared<Sphere>(point3(200, 20, 200)  white));

  // Light
  world.add(make_shared<quad>(point3(200, 554, 200), vec3(200, 0, 0), vec3(0, 0, 200), light));
  world.add(make_shared<sphere>(point3(350, 600, 200), 200, light));

  // Box

  // auto test_box = make_shared<quad>(point3(0, 0, 400), vec3(0, 0, -400), vec3(0, 555, 0), red);
  // auto test_box_r = make_shared<rotate_y>(test_box, 90);
  // world.add(make_shared<translate>(test_box_r, vec3(0, 0, 400)));
  // shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(100, 400, 100), white);
  // box1 = make_shared<translate>(box1, vec3(140, 0, 140));
  // world.add(box1);
  // world.add(make_shared<rotate_y>(box1, -45));
  // world.add(make_shared<rotate_z>(box1, 45));

  // auto box2 = make_shared<rotate_y>(box1, 15);
  // box2 = make_shared<translate>(box2, vec3(-100, 0, -100));

  // world.add(box2);
  // box1 = make_shared<translate>(box1, vec3(265, 0, 295));
  // world.add(make_shared<constant_medium>(box1, 0.001, color(0.25, 0.25, 0.25)));

  // Glass Sphere

  world.add(make_shared<sphere>(point3(200, 90, 190), 90, glass));

  return world;
}
