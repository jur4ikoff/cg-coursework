#include "task.h"

#include <iostream>
#include <memory>
#include "render.h"

void RenderTask::execute(ColorMatrix &color_matrix, const Tile &tile,
                         const Hittable &world, volatile bool &cancel_running)
{
  for (int i = tile.i; i < tile.end_i; i++)
  {
    if (cancel_running)
      break;
    for (int j = tile.j; j < tile.end_j; j++)
    {
      if (cancel_running)
        break;
      Color pixel_color(0, 0, 0);
      for (int sample = 0; sample < _camera.samples_per_pixel; sample++)
      {
        if (cancel_running)
          break;
        Ray r = _camera.get_ray(j, i);
        pixel_color += _camera.ray_color(r, _camera.max_depth, world, cancel_running);
      }
      color_matrix.at(i, j) = _camera.pixel_samples_scale * pixel_color;
    }
  }
}
