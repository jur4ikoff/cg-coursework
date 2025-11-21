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
  HittableList world;

  auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
  world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

  for (int a = -2; a < 2; a++) {
    for (int b = -2; b < 2; b++) {
      auto choose_mat = random_double();
      Point3 center(a + 0.8 * random_double(), 0.2, b + 0.8 * random_double());

      if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
        shared_ptr<Material> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = Color::random() * Color::random();
          sphere_material = make_shared<Lambertian>(albedo);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        } else if (choose_mat < 0.95) {
          // metal
          auto albedo = Color::random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = make_shared<Metal>(albedo, fuzz);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        } else {
          // glass
          sphere_material = make_shared<Dielectric>(1.5);
          world.add(make_shared<Sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
  world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material2));
  world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

  auto material3 = make_shared<Metal>(Color(0.7, 0.7, 0.8), 0.2);
  world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

  world.add(make_shared<Sphere>(Point3(6, 2, 0.5), 1, material3));

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

  auto start = std::chrono::high_resolution_clock::now();
  render.render(world, color_matrix, cancel_running, 8, tile_callback);
  auto end = std::chrono::high_resolution_clock::now();
  auto duration =
      std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  std::cout << duration.count() << " milliseconds\n";
  // render.render_seq(world, color_matrix);
}

void Scene::measure_1(ColorMatrix &color_matrix) {
  std::cout << "Meausure\n";

  std::ofstream file(DATA_PATH_1EXP);
  if (!file.is_open()) {
    throw std::runtime_error("ОШИБКА!!! Файл В С Ë");
  }

  bool flag = false;
  int exp_count = 20;
  int obj_count = 20;

  HittableList world;
  Render render;
  render.samples_per_pixel = 1;
  render.max_depth = 10;

  render.vfov = 30;
  render.lookfrom = Point3(13, 2, -3);
  render.lookat = Point3(0, 0, 0);
  render.vup = Vec3(0, 1, 0);

  render.defocus_angle = 0;
  render.focus_dist = 5.0;

  std::shared_ptr<Material> material;

  auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
  world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

  file << "obj_count;" << "seq_time;" << "parallel_time" << std::endl;
  for (size_t i = 0; i < obj_count; i += 2) {
    Point3 center(random_double(-5, 15), 0.35, random_double(-4.5, 4.5));
    int material_type = i % 3;
    if (i == 0)
      continue;
    if (i % 3 == 0) {
      auto albedo = Color::random(0.5, 1);
      auto fuzz = random_double(0, 0.5);
      material = make_shared<Metal>(albedo, fuzz);
    }
    if (i % 3 == 1) {
      material = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    }
    if (i % 3 == 2) {
      material = make_shared<Metal>(Color(1, 1, 1), 0.5);
    }

    world.add(make_shared<Sphere>(center, 0.4, material));

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t k = 0; k < exp_count; k++) {
      render.render_seq(world, color_matrix);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto seq =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    start = std::chrono::high_resolution_clock::now();
    for (size_t k = 0; k < exp_count; k++) {
      render.render(world, color_matrix, flag, 1, nullptr);
    }
    end = std::chrono::high_resolution_clock::now();
    auto parallel =
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    file << i << ";" << seq.count() / exp_count << ";"
         << parallel.count() / exp_count << std::endl;
    std::cout << i << ";" << seq.count() / exp_count << ";"
              << parallel.count() / exp_count << std::endl;
  }
}

void Scene::measure_2(ColorMatrix &color_matrix) {
  std::cout << "Meausure 2\n";

  std::ofstream file(DATA_PATH_2EXP);
  if (!file.is_open()) {
    throw std::runtime_error("ОШИБКА!!! Файл В С Ë");
  }

  bool flag = false;
  int exp_count = 10;

  Render render;
  render.samples_per_pixel = 40;
  render.max_depth = 10;

  render.vfov = 30;
  render.lookfrom = Point3(13, 2, -3);
  render.lookat = Point3(0, 0, 0);
  render.vup = Vec3(0, 1, 0);

  render.defocus_angle = 0;
  render.focus_dist = 5.0;

  std::shared_ptr<Material> material;

  // auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
  // world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

  file << "thread_count;" << "1;2;3;5;10;15;20;25" << std::endl;

  int max_object_count = 30;
  std::vector<int> object_counts{1, 2, 3, 5, 10, 15, 20, 25};
  std::vector<int> thread_counts{1, 2, 4, 8, 16, 24, 32, 64, 128};

  for (int thread_count : thread_counts) {
    std::cout << thread_count << ";";
    file << thread_count << ";";
    HittableList world;
    for (int i = 0; i < max_object_count; i++) {
      Point3 center(random_double(-5, 15), 0.35, random_double(-4.5, 4.5));
      int material_type = i % 3;
      if (i % 3 == 0) {
        auto albedo = Color::random(0.5, 1);
        auto fuzz = random_double(0, 0.5);
        material = make_shared<Metal>(albedo, fuzz);
      }
      if (i % 3 == 1) {
        material = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
      }
      if (i % 3 == 2) {
        material = make_shared<Metal>(Color(1, 1, 1), 0.5);
      }

      world.add(make_shared<Sphere>(center, 0.4, material));

      if (std::find(object_counts.begin(), object_counts.end(), i + 1) !=
          object_counts.end())

      {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t k = 0; k < exp_count; k++) {
          render.render(world, color_matrix, flag, thread_count, nullptr);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto seq =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        if (i != max_object_count - 1) {
          std::cout << seq.count() / exp_count << ";";
          file << seq.count() / exp_count << ";";
        } else {
          std::cout << seq.count() / exp_count;
          file << seq.count() / exp_count;
        }
      }
    }
    std::cout << "\n";
    file << "\n";
  }
}