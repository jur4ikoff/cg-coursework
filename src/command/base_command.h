#pragma once

#include "render_manager.h"

class BaseCommand
{
public:
    BaseCommand() = default;
    virtual ~BaseCommand() = 0;

    void set_managers(std::shared_ptr<RenderManager> render_manager);

    virtual void execute() = 0;

protected:
    std::shared_ptr<RenderManager> _render_manager;
};
