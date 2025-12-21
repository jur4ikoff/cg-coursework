#pragma once

#include "base_command.h"
#include "render_manager.h"

class Facade {
public:
  Facade();
  ~Facade() = default;

  void execute(BaseCommand &command);

private:
  std::shared_ptr<RenderManager> _render_manager;
};