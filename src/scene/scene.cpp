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

  auto red = make_shared<Lambertian>(Color(.65, .05, .05));
  auto white = make_shared<Lambertian>(Color(.73, .73, .73));
  auto green = make_shared<Lambertian>(Color(.12, .45, .15));
  auto light = make_shared<diffuse_light>(Color(5, 5, 5));
  auto cylinder_mat = make_shared<Lambertian>(Color(1, 0, 0));
  auto glass = make_shared<Transparent>(1.5);
  auto metal_mat = make_shared<Metal>(Color(0.8, 0.8, 0.8), 1);
  auto fog_color = make_shared<Color>(0.1, 0.1, 0.1);
  auto nz_tex = make_shared<NoiseTexture>(1);
  auto nz_mat = make_shared<Lambertian>(nz_tex);

  auto noise = std::make_shared<Perlin>();

  // Граница — например, большой box
  // auto boundary = make_shared<Quad>(point3(0, 0, 0), vec3(400, 0, 400), vec3(400, 555, 400), red);
  // auto boundary = make_shared<Quad>(point3(0, 0, 400), vec3(0, 0, -400), vec3(0, 555, 0), red);
  auto boundary = box(point3(-10, -10, 0), point3(700, 700, 4000), white);

  // _objects.add(make_shared<ConstantFog>(boundary, 0.003, *fog_color));
  // _objects.add(make_shared<Smoke>(boundary, 2, 0.01, *fog_color));
  // _objects.add(make_shared<NonUniformFog>(boundary, *fog_color));
  // _objects.add(make_shared<DynamicFog>(boundary, 0.01, noise, 0.5, *fog_color));

  // _objects.add(make_shared<Cone>(point3(200, 0, 350), 100, 200, white));
  _objects.add(box(point3(0, 0, 600), point3(100, 500, 10000), green));

  // auto pyr = pyramid(point3(400, 0, 200), 150, 300, red);
  // auto pyr_r = make_shared<RotateY>(pyr, -45);
  // _objects.add(pyr_r);

  // auto pyr = irregular_pyramid(point3(0,0,0), point3(0, 0, 200), point3(200, 0, 200), point3(200, 0, 0), point3(200, 400, 100), metal_mat);
  // _objects.add(pyr);

  // auto pyr = triangle_pyramid(point3(0, 0, 0), point3(0, 0, 200), point3(200, 0, 200), point3(100, 200, 100), metal_mat);
  // _objects.add(pyr);

  // auto cylinder_1 = make_shared<Cylinder>(50, 0, 100, cylinder_mat);

  // auto tr = make_shared<triangle>(point3(0, 0, 0), point3(100, 400, 100), point3(200, 0, 200), white);
  // auto tr_r = make_shared<RotateY>(tr, 90);
  // _objects.add(make_shared<Shift>(tr_r, vec3(0, 0, 400)));

  // Cornell box sides
  _objects.add(make_shared<Quad>(point3(555, 0, 0), Vec3(0, 0, 555), Vec3(0, 555, 0), green));
  _objects.add(make_shared<Quad>(point3(0, 0, 555), Vec3(0, 0, -555), Vec3(0, 555, 0), red));
  // _objects.add(make_shared<Quad>(point3(0,555,0), vec3(555,0,0), vec3(0,0,555), white));
  _objects.add(make_shared<Quad>(point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 0, -555), white));
  // _objects.add(make_shared<Quad>(point3(555, 0, 555), Vec3(-555, 0, 0), Vec3(0, 555, 0), white));

  // _objects.add(make_shared<Sphere>(point3(200, 20, 200)  white));

  // Light
  _objects.add(make_shared<Quad>(point3(200, 554, 200), Vec3(200, 0, 0), Vec3(0, 0, 200), light));
  // _objects.add(make_shared<Sphere>(point3(250, 700, 500), 200, light));

  // Box

  // auto test_box = make_shared<Quad>(point3(0, 0, 400), vec3(0, 0, -400), vec3(0, 555, 0), red);
  // auto test_box_r = make_shared<RotateY>(test_box, 90);
  // _objects.add(make_shared<Shift>(test_box_r, vec3(0, 0, 400)));
  // shared_ptr<hittable> box1 = box(point3(0, 0, 0), point3(100, 400, 100), white);
  // box1 = make_shared<Shift>(box1, vec3(140, 0, 140));
  // _objects.add(box1);
  // _objects.add(make_shared<RotateY>(box1, -45));
  // _objects.add(make_shared<RotateZ>(box1, 45));

  // auto box2 = make_shared<RotateY>(box1, 15);
  // box2 = make_shared<Shift>(box2, vec3(-100, 0, -100));

  // _objects.add(box2);
  // box1 = make_shared<Shift>(box1, vec3(265, 0, 295));
  // _objects.add(make_shared<ConstantFog>(box1, 0.001, Color(0.25, 0.25, 0.25)));

  _objects.add(make_shared<Sphere>(point3(200, 90, 190), 90, glass));

  return _objects;
}

void Scene::make_default_scene()
{
}

HittableList &Scene::get_objects()
{
  return _objects;
}