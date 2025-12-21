
#include "render_manager.h"
#include "render.h"

#include <iostream>

RenderManager::RenderManager() : _render(std::make_shared<Render>()) {}

void RenderManager::start_render(RenderSettings &render_settings,
                                 Camera &camera, const Hittable &objects,
                                 ColorMatrix &color_matrix,
                                 volatile bool &cancel_running) {
  Render render;
  render.set_camera(camera);

  render.background = render_settings.background;
  render.max_depth = render_settings.max_depth;
  render.samples_per_pixel = render_settings.samples_per_pixel;

  render.render(objects, color_matrix, cancel_running, 8, nullptr);
}