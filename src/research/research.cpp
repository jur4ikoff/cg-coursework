#include "research.h"
// #include "constants.h"
#include "camera.h"
#include "color_matrix.h"
#include "render.h"
#include "scene.h"
#include "task.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <memory>
#include <stdexcept>

static std::shared_ptr<Camera> init_camera() {
  auto camera = std::make_shared<Camera>();
  camera->id = 0;
  camera->vfov = 37;
  camera->lookfrom = Point3(278, 278, -800);
  camera->lookat = Point3(278, 278, 0);
  camera->vup = Vec3(0, 1, 0);
  camera->defocus_angle = 0;
  camera->focus_dist = 10;

  return camera;
}

void run_sample_count_exp(size_t start, size_t end, size_t step) {
  std::ofstream file(SAMPLE_EXP);
  if (!file.is_open()) {
    throw std::runtime_error("ОШИБКА!!! Файл не найден");
  }

  auto scene = std::make_shared<Scene>();
  auto color_matrix = std::make_shared<ColorMatrix>(IMG_SIZE, IMG_SIZE);
  auto world = scene->make_default_scene();
  auto render = std::make_shared<Render>();
  auto camera = init_camera();
  volatile bool flag = false;

  RenderSettings settings{(int)start, 10, Color{0, 0, 0}};
  render->set_render_settings(settings);

  std::cout << "samples_count;" << "time" << "\n";
  file << "samples_count;" << "time" << "\n";

  for (size_t i = start; i <= end; i += step) {
    render->samples_per_pixel = i;
    auto time_start = std::chrono::high_resolution_clock::now();
    for (size_t j = 0; j < EXP_COUNT; j++) {
      render->render(world, *color_matrix, flag, 8, nullptr);
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        time_end - time_start);
    std::cout << i << ";" << duration.count() / EXP_COUNT << "\n";
    file << i << ";" << duration.count() / EXP_COUNT << "\n";
  }
}

void run_depth_exp(size_t start, size_t end, size_t step) {
  std::ofstream file(DEPTH_EXP);
  if (!file.is_open()) {
    throw std::runtime_error("ОШИБКА!!! Файл не найден");
  }

  auto scene = std::make_shared<Scene>();
  auto color_matrix = std::make_shared<ColorMatrix>(IMG_SIZE, IMG_SIZE);
  auto world = scene->make_default_scene();
  auto render = std::make_shared<Render>();
  auto camera = init_camera();
  volatile bool flag = false;

  RenderSettings settings{10, (int)start, Color{0, 0, 0}};
  render->set_render_settings(settings);

  std::cout << "depth;" << "time" << "\n";
  file << "depth;" << "time" << "\n";

  for (size_t i = start; i <= end; i += step) {
    render->max_depth = i;
    auto time_start = std::chrono::high_resolution_clock::now();
    for (size_t j = 0; j < EXP_COUNT; j++) {
      render->render(world, *color_matrix, flag, 8, nullptr);
    }
    auto time_end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        time_end - time_start);
    std::cout << i << ";" << duration.count() / EXP_COUNT << "\n";
    file << i << ";" << duration.count() / EXP_COUNT << "\n";
  }
}