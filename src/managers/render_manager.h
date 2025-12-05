#pragma once

#include <memory>
#include "scene.h"
#include "render.h"

class RenderManager
{
    protected: 
    std::shared_ptr<Render> _render;
public:
    RenderManager();
    ~RenderManager() = default;

    void start_render(RenderSettings &render_settings, Camera &camera, const Hittable &objects, ColorMatrix &color_matrix,
                             volatile bool &cancel_running);
};
