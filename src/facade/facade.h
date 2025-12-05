#pragma once

#include "render_manager.h"
#include "base_command.h"

class Facade
{
    public:
    Facade();
    ~Facade() = default;

    void execute(BaseCommand &command);
private:
    std::shared_ptr<RenderManager> _render_manager;
};