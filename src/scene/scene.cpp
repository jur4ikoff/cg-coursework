#include "scene.h"
#include "color.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "ray.h"
#include "render.h"
#include "sphere.h"
#include "vec3.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <memory>
#include <stdexcept>

#define DATA_PATH_1EXP "./../charts/exp_1.csv"
#define DATA_PATH_2EXP "./../charts/exp_2.csv"

void Scene::draw(QSize size, ColorMatrix &color_matrix,
                 volatile bool &cancel_running,
                 std::function<void()> tile_callback) {
  // HittableList world;

  // auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
  // world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

  // for (int a = -2; a < 2; a++) {
  //   for (int b = -2; b < 2; b++) {
  //     auto choose_mat = random_double();
  //     Point3 center(a + 0.8 * random_double(), 0.2, b + 0.8 * random_double());

  //     if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
  //       shared_ptr<Material> sphere_material;

  //       if (choose_mat < 0.8) {
  //         // diffuse
  //         auto albedo = Color::random() * Color::random();
  //         sphere_material = make_shared<Lambertian>(albedo);
  //         world.add(make_shared<Sphere>(center, 0.2, sphere_material));
  //       } else if (choose_mat < 0.95) {
  //         // metal
  //         auto albedo = Color::random(0.5, 1);
  //         auto fuzz = random_double(0, 0.5);
  //         sphere_material = make_shared<Metal>(albedo, fuzz);
  //         world.add(make_shared<Sphere>(center, 0.2, sphere_material));
  //       } else {
  //         // glass
  //         sphere_material = make_shared<Dielectric>(1.5);
  //         world.add(make_shared<Sphere>(center, 0.2, sphere_material));
  //       }
  //     }
  //   }
  // }

  // auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
  // world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material2));
  // world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

  // auto material3 = make_shared<Metal>(Color(0.7, 0.7, 0.8), 0.2);
  // world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

  // world.add(make_shared<Sphere>(Point3(6, 2, 0.5), 1, material3));

  // Render render;
  Render render;
  render.samples_per_pixel = 40;
  render.max_depth = 10;

  render.vfov = 30;
  render.lookfrom = Point3(13, 2, -3);
  render.lookat = Point3(0, 0, 0);
  render.vup = Vec3(0, 1, 0);

  render.defocus_angle = 0;
  render.focus_dist = 5.0;

  // auto start = std::chrono::high_resolution_clock::now();
  // render.render(world, color_matrix, cancel_running, 8, tile_callback);
  // auto end = std::chrono::high_resolution_clock::now();
  // auto duration =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  // std::cout << duration.count() << " milliseconds\n";
  // render.render_seq(world, color_matrix);
}
