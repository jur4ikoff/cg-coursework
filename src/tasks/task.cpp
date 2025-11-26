#include "task.h"

#include <iostream>
#include <memory>
#include "render.h"

void RenderTask::execute(ColorMatrix &color_matrix, const Tile &tile,
                         const hittable &world) {
  // update
  for (int i = tile.i; i < tile.end_i; i++) {
    for (int j = tile.j; j < tile.end_j; j++) {
      color pixel_color(0, 0, 0);
      for (int sample = 0; sample < _camera.samples_per_pixel; sample++) {
        ray r = _camera.get_ray(j, i);
        pixel_color += _camera.ray_color(r, _camera.max_depth, world);
      }
      color_matrix.at(i, j) = _camera.pixel_samples_scale * pixel_color;
    }
  }
}

