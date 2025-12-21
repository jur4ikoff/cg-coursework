#pragma once

#include "base_command.h"
// #include "camera.h"
// #include "color_matrix.h"
// #include "hittable.h"

#include <iostream>

class StartSimpleRenderCommand : public BaseCommand {
public:
  StartSimpleRenderCommand() = delete;
  StartSimpleRenderCommand(RenderSettings &render_settings, Camera &camera,
                           const Hittable &objects, ColorMatrix &color_matrix,
                           volatile bool &cancel_running)
      : _render_settings(render_settings), _camera(camera), _objects(objects),
        _color_matrix(color_matrix), _cancel_running(cancel_running) {}

  virtual ~StartSimpleRenderCommand() = default;

  virtual void execute() override {
    _render_manager->start_render(_render_settings, _camera, _objects,
                                  _color_matrix, _cancel_running);
  }

private:
  RenderSettings &_render_settings;
  Camera &_camera;
  const Hittable &_objects;
  ColorMatrix &_color_matrix;
  volatile bool &_cancel_running;
};