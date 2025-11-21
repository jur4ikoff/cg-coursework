#include "task.h"

#include <iostream>
#include <memory>

void RenderTask::execute(ColorMatrix &color_matrix, const Tile &tile,
                         const Hittable &world) {
  // auto start = std::chrono::high_resolution_clock::now();
  for (int i = tile.i; i < tile.end_i; i++) {
    for (int j = tile.j; j < tile.end_j; j++) {
      Color pixel_color(0, 0, 0);
      for (int sample = 0; sample < _camera.samples_per_pixel; sample++) {
        Ray ray = _camera.get_ray(j, i);
        pixel_color += _camera.ray_color(ray, _camera.max_depth, world);
      }
      color_matrix.at(i, j) = _camera.pixel_samples_scale * pixel_color;
    }
  }

  // auto end = std::chrono::high_resolution_clock::now();
  // auto duration =
  //     std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
  // std::cout << duration.count() << " milliseconds task\n";
}

std::vector<std::shared_ptr<Tile>> TileCreator::create(int step) {
  if (step < 20) {
    step = 20;
  }
  std::vector<std::shared_ptr<Tile>> tiles;
  for (int i = 0; i < _size_x; i += step) {
    for (int j = 0; j < _size_y; j += step) {

      auto tile = std::make_shared<Tile>(i, j, std::min(i + step, _size_x),
                                         std::min(j + step, _size_y));
      tiles.push_back(tile);
    }
  }
  return tiles;
}