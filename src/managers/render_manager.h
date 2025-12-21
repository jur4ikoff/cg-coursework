#pragma once

#include "render.h"
#include "scene.h"
#include <memory>

class RenderManager {
protected:
  std::shared_ptr<Render> _render;

public:
  RenderManager();
  ~RenderManager() = default;

  void start_render(RenderSettings &render_settings, Camera &camera,
                    const Hittable &objects, ColorMatrix &color_matrix,
                    volatile bool &cancel_running);
};
