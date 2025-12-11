#include "scene.h"

#include "cone.h"
#include "cylinder.h"
#include "fog.h"
#include "hittable_list.h"
#include "pyramid.h"
#include "quad.h"
#include "render.h"
#include "sphere.h"

Scene::Scene()
{
  _default_material = std::make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
}

// Сделать эту функцию так, чтобы возвращалась сцена через параметр
HittableList Scene::make_default_scene()
{

  auto white = make_shared<Lambertian>(Color(0.8, 0.8, 0.8));
  auto green = make_shared<Lambertian>(Color(0.12, 0.45, 0.15));
  auto light_blue = make_shared<Lambertian>(Color(0, 0.491, 1));
  auto red = make_shared<Lambertian>(Color(1, 0, 0.237));

  auto light = make_shared<diffuse_light>(Color(5, 5, 5));
  auto cylinder_mat = make_shared<Lambertian>(Color(1, 0, 0));
  auto glass = make_shared<Transparent>(1.5);
  auto metal_mat = make_shared<Metal>(Color(0.8, 0.8, 0.8), 1);
  auto fog_color = make_shared<Color>(1, 1, 1);


  // Граница — например, большой box
  // auto boundary = box(Point3(-10, -10, 0), Point3(600, 600, 600), white);
  auto test = box(Point3(0, 0, 0), Point3(600, 600, 600), white);
  // _objects.add(test);

  // _objects.add(make_shared<ConstantFog>(boundary, 0.003, *fog_color));
  _objects.add(make_shared<Smoke>(test, 2, 0.004, *fog_color));
  // _objects.add(make_shared<GroundSmoke>(test, 3, 0.0025, 0.0028, *fog_color));

  _objects.add(make_shared<Cone>(Point3(450, 0, 350), 100, 200, white));
  _objects.add(make_shared<Cone>(Point3(100, 0, 100), 50, 100, red));

  // auto pyr = pyramid(Point3(400, 0, 200), 150, 300, red);
  // auto pyr_r = make_shared<RotateY>(pyr, -45);
  // _objects.add(pyr_r);

  // auto pyr = irregular_pyramid(Point3(0,0,0), Point3(0, 0, 200), Point3(200, 0, 200), Point3(200, 0, 0), Point3(200, 400, 100), metal_mat);
  // _objects.add(pyr);

  // auto pyr = triangle_pyramid(Point3(0, 0, 0), Point3(0, 0, 200), Point3(200, 0, 200), Point3(100, 200, 100), metal_mat);
  // _objects.add(pyr);

  // auto cylinder_1 = make_shared<Cylinder>(50, 0, 100, cylinder_mat);

  // auto tr = make_shared<triangle>(Point3(0, 0, 0), Point3(100, 400, 100), Point3(200, 0, 200), white);
  // auto tr_r = make_shared<RotateY>(tr, 90);
  // _objects.add(make_shared<Shift>(tr_r, vec3(0, 0, 400)));

  // Cornell box sides
  _objects.add(make_shared<Quad>(Point3(555, 0, 0), Vec3(0, 0, 555), Vec3(0, 555, 0), light_blue));
  _objects.add(make_shared<Quad>(Point3(0, 0, 555), Vec3(0, 0, -555), Vec3(0, 555, 0), red));
  _objects.add(make_shared<Quad>(Point3(0, 555, 0), Vec3(555, 0, 0), Vec3(0, 0, 555), white));
  _objects.add(make_shared<Quad>(Point3(0, 0, 555), Vec3(555, 0, 0), Vec3(0, 0, -555), white));
  _objects.add(make_shared<Quad>(Point3(555, 0, 555), Vec3(-555, 0, 0), Vec3(0, 555, 0), white));

  // _objects.add(make_shared<Sphere>(Point3(200, 20, 200)  white));

  // Light
  _objects.add(make_shared<Quad>(Point3(200, 554, 200), Vec3(200, 0, 0), Vec3(0, 0, 200), light));
  _objects.add(make_shared<Quad>(Point3(200, 1, 200), Vec3(200, 0, 0), Vec3(0, 0, 200), light));

  // _objects.add(make_shared<Sphere>(Point3(250, 700, 500), 200, light));

  // Box

  // auto test_box = make_shared<Quad>(Point3(0, 0, 400), vec3(0, 0, -400), vec3(0, 555, 0), red);
  // auto test_box_r = make_shared<RotateY>(test_box, 90);
  // _objects.add(make_shared<Shift>(test_box_r, vec3(0, 0, 400)));
  // shared_ptr<hittable> box1 = box(Point3(0, 0, 0), Point3(100, 400, 100), white);
  // box1 = make_shared<Shift>(box1, vec3(140, 0, 140));
  // _objects.add(box1);
  // _objects.add(make_shared<RotateY>(box1, -45));
  // _objects.add(make_shared<RotateZ>(box1, 45));

  // auto box2 = make_shared<RotateY>(box1, 15);
  // box2 = make_shared<Shift>(box2, vec3(-100, 0, -100));

  // _objects.add(box2);
  // box1 = make_shared<Shift>(box1, vec3(265, 0, 295));
  // _objects.add(make_shared<ConstantFog>(box1, 0.001, Color(0.25, 0.25, 0.25)));

  _objects.add(make_shared<Sphere>(Point3(200, 90, 190), 90, glass));

  return _objects;
}

void Scene::add_sphere(const Point3 &center, double radius)
{
  _objects.add(make_shared<Sphere>(center, radius, _default_material));
}

void Scene::add_cylinder(const Point3 &center, double radius, double ymin, double ymax)
{
  _objects.add(make_shared<Cylinder>(center, radius, ymin, ymax, _default_material));
}

void Scene::add_cone(const Point3 &center, double radius, double height)
{
  _objects.add(make_shared<Cone>(center, radius, height, _default_material));
}

void Scene::add_quad_pyramid(const Point3 &center, double half_width, double height)
{
  _objects.add(pyramid(center, half_width, height, _default_material));
}

void Scene::add_triag_pyramid(const Point3 &v0, const Point3 &v1, const Point3 &v2, const Point3 &apex)
{
  _objects.add(triangle_pyramid(v0, v1, v2, apex, _default_material));
}

void Scene::add_box(const Point3 &point1, const Point3 &point2)
{
  _objects.add(box(point1, point2, _default_material));
}

void Scene::add_quad(const Point3 &left_down, const Vec3 &u, const Vec3 &v)
{
  _objects.add(make_shared<Quad>(left_down, u, v, _default_material));
}

void Scene::add_triangle(const Point3 &v1, const Point3 &v2, const Point3 &v3)
{
  _objects.add(make_shared<Triangle>(v1, v2, v3, _default_material));
}

void Scene::add_light(size_t id, double power, Color &color)
{
  auto object = _objects.get_object_by_id(id);
  color *= power;

  if (object)
  {

    auto light = make_shared<diffuse_light>(color);
    object->mat = light;
  }
  std::cout << object->id << std::endl;
}

void Scene::delete_object(size_t id)
{
  _objects.delete_object(id);
}

HittableList &Scene::get_objects()
{
  return _objects;
}

std::vector<size_t> Scene::get_objects_ids()
{
  return _objects.get_objects_ids();
}

void Scene::set_material(size_t id, MaterialStruct &mat_struct)
{
  auto object = _objects.get_object_by_id(id);
  if (!object)
    throw std::invalid_argument("Нету такого объекта");

  std::shared_ptr<Material> mat = nullptr;

  switch (mat_struct.type)
  {
  case Lambertian_t:
    mat = std::make_shared<Lambertian>(mat_struct.color);
    break;
  case Metal_t:
    mat = std::make_shared<Metal>(mat_struct.color, mat_struct.fuzz);
    break;
  case Transparent_t:
    mat = std::make_shared<Transparent>(mat_struct.refraction_index);
    break;
  default:
    throw std::invalid_argument("Unknown material type");
  }

  object->mat = mat;
}

void Scene::move_object(size_t id, Vec3 &offset)
{
  auto object = _objects.get_object_by_id(id);
  if (!object)
    throw std::invalid_argument("Нету такого объекта");

  if (offset.x() == 0 && offset.y() == 0 && offset.z() == 0)
    return;

  std::cout << offset.x() << " " << offset.y() << " " << offset.z() << std::endl;
  auto new_object = std::make_shared<Shift>(object, offset);
  _objects.add_element_instead_of_id(id, new_object);
}

void Scene::rotate_object(size_t id, Vec3 &rotate_info)
{
  auto object = _objects.get_object_by_id(id);
  if (!object)
    throw std::invalid_argument("Нету такого объекта");

  if (rotate_info.x() == 0 && rotate_info.y() == 0 && rotate_info.z() == 0)
    return;

  std::shared_ptr<Hittable> new_object = std::make_shared<RotateX>(object, rotate_info.x());
  new_object = std::make_shared<RotateY>(new_object, rotate_info.y());
  new_object = std::make_shared<RotateZ>(new_object, rotate_info.z());
  _objects.add_element_instead_of_id(id, new_object);
}

void Scene::add_constant_fog(size_t obj_id, double density, Color &fog_color)
{
  auto object = _objects.get_object_by_id(obj_id);
  _objects.add(make_shared<ConstantFog>(object, density, fog_color));
  _objects.delete_object(obj_id);
}

void Scene::add_smoke(size_t obj_id, double density, double scale, Color &fog_color)
{
  auto object = _objects.get_object_by_id(obj_id);
  _objects.add(make_shared<Smoke>(object, density, scale, fog_color));
  _objects.delete_object(obj_id);
}

void Scene::add_ground_smoke(size_t obj_id, double density, double scale, double height_falloff, Color &fog_color)
{
  auto object = _objects.get_object_by_id(obj_id);
  _objects.add(make_shared<GroundSmoke>(object, density, scale, height_falloff, fog_color));
  _objects.delete_object(obj_id);
}

void Scene::change_visibility(size_t id)
{
  shared_ptr<Hittable> &object = _objects.get_object_by_id(id);
  object->is_visible = !object->is_visible;
}