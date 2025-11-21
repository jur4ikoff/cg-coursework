#include "color.h"
#include "color_matrix.h"
#include "constants.h"
#include "hittable.h"
#include "material.h"
#include "render.h"
#include "vec3.h"

#include <iostream>

class Tile {

public:
  Tile(int _i, int _j, int _end_i, int _end_j) {
    i = _i;
    j = _j;
    end_i = _end_i;
    end_j = _end_j;
  }
  int i;
  int j;
  int end_i;
  int end_j;
};

class TileCreator {
public:
  TileCreator(int size_x, int size_y, int step = 100)
      : _size_x(size_x), _size_y(size_y) {}
  std::vector<std::shared_ptr<Tile>> create(int step);

private:
  int _size_x;
  int _size_y;
};

class RenderTask {
public:
  explicit RenderTask(Render &camera) : _camera(camera) {}
  ~RenderTask() = default;
  void execute(ColorMatrix &color_matrix, const Tile &tile,
               const Hittable &world);

private:
  const Render &_camera;
};
