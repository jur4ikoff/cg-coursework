#include "scene.h"

#include "hittable_list.h"
#include "render.h"
#include "sphere.h"
#include "quad.h"
#include "pyramid.h"
#include "cylinder.h"
#include "cone.h"
#include "fog.h"

// Сделать эту функцию так, чтобы возвращалась сцена через параметр
HittableList Scene::draw()
{
  HittableList world;

  auto red = make_shared<Lambertian>(color(.65, .05, .05));
  auto white = make_shared<Lambertian>(color(.73, .73, .73));
  auto green = make_shared<Lambertian>(color(.12, .45, .15));
  auto light = make_shared<diffuse_light>(color(5, 5, 5));
  auto cylinder_mat = make_shared<Lambertian>(color(1, 0, 0));
  auto glass = make_shared<Transparent>(1.5);
  auto metal_mat = make_shared<Metal>(color(0.8, 0.8, 0.8), 1);
  auto fog_color = make_shared<color>(0.2, 0.2, 0.2);
  auto nz_tex = make_shared<NoiseTexture>(1);
  auto nz_mat = make_shared<Lambertian>(nz_tex);

  auto noise = make_shared<Perlin>();

  // Граница — например, большой box
  // auto boundary = make_shared<Quad>(point3(0, 0, 0), vec3(400, 0, 400), vec3(400, 555, 400), red);
  // auto boundary = make_shared<Quad>(point3(0, 0, 400), vec3(0, 0, -400), vec3(0, 555, 0), red);
  // auto boundary = box(point3(0, 0, -100), point3(700, 700, 700), white);
  // auto boundary_2 = box(point3(275, 0, 0), point3(550, 600, 600), white);

  // world.add(make_shared<ConstantFog>(boundary_2, 0.001, *fog_color));
  // world.add(make_shared<NonuniformFog>(boundary, noise, 2.5, 0.02, *fog_color));

  // world.add(make_shared<Cone>(point3(200, 0, 350), 100, 200, white));
  // world.add(box(point3(200, 0, 100), point3(400, 200, 500), green));

  // auto pyr = pyramid(point3(400, 0, 200), 150, 300, red);
  // auto pyr_r = make_shared<RotateY>(pyr, -45);
  // world.add(pyr_r);

  // auto pyr = irregular_pyramid(point3(0,0,0), point3(0, 0, 200), point3(200, 0, 200), point3(200, 0, 0), point3(200, 400, 100), metal_mat);
  // world.add(pyr);

  // auto pyr = triangle_pyramid(point3(0, 0, 0), point3(0, 0, 200), point3(200, 0, 200), point3(100, 200, 100), metal_mat);
  // world.add(pyr);

  // auto cylinder_1 = make_shared<Cylinder>(50, 0, 100, cylinder_mat);

  // auto tr = make_shared<triangle>(point3(0, 0, 0), point3(100, 400, 100), point3(200, 0, 200), white);
  // auto tr_r = make_shared<RotateY>(tr, 90);
  // world.add(make_shared<Shift>(tr_r, vec3(0, 0, 400)));

  // Cornell box sides
  world.add(make_shared<Quad>(point3(555, 0, 0), Vec3(0, 0, 555), Vec3(0, 555, 0), green));
  world.add(make_shared<Quad>(point3(0, 0, 555), Vec3(0, 0, -555), Vec3(0, 555, 0), red));
  // world.add(make_shared<Quad>(point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
  world.add(make_shared<Quad>(point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 0, -555), white));
  world.add(make_shared<Quad>(point3(555, 0, 555), Vec3(-555, 0, 0), Vec3(0, 555, 0), white));
  // world.add(make_shared<Sphere>(point3(200, 20, 200)  white));

  // Light
  world.add(make_shared<Quad>(point3(200, 554, 200), Vec3(200, 0, 0), Vec3(0, 0, 200), light));
  // world.add(make_shared<Sphere>(point3(250, 700, 500), 200, light));

  // Box

  // auto test_box = make_shared<Quad>(point3(0, 0, 400), vec3(0, 0, -400), vec3(0, 555, 0), red);
  // auto test_box_r = make_shared<RotateY>(test_box, 90);
  // world.add(make_shared<Shift>(test_box_r, vec3(0, 0, 400)));
  // shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(100, 400, 100), white);
  // box1 = make_shared<Shift>(box1, vec3(140, 0, 140));
  // world.add(box1);
  // world.add(make_shared<RotateY>(box1, -45));
  // world.add(make_shared<RotateZ>(box1, 45));

  // auto box2 = make_shared<RotateY>(box1, 15);
  // box2 = make_shared<Shift>(box2, vec3(-100, 0, -100));

  // world.add(box2);
  // box1 = make_shared<Shift>(box1, vec3(265, 0, 295));
  // world.add(make_shared<ConstantFog>(box1, 0.001, color(0.25, 0.25, 0.25)));

  // Glass Sphere

  // world.add(make_shared<Sphere>(point3(200, 90, 190), 90, glass));

  return world;
}
