#pragma once

#include "color.h"
#include "color_matrix.h"
#include "constants.h"
#include "hittable.h"
#include "tile_creator.h"
#include "material.h"
// #include "render.h"
#include "vec3.h"

#include <iostream>


class Render;

class RenderTask
{
public:
    explicit RenderTask(Render &cam) : _camera(cam) {}
    ~RenderTask() = default;
    void execute(ColorMatrix &color_matrix, const Tile &tile,
                 const hittable &world);

private:
    const Render &_camera;
};
