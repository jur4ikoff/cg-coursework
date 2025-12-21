#include "facade.h"

Facade::Facade() { _render_manager = std::make_shared<RenderManager>(); }

void Facade::execute(BaseCommand &command) {
  command.set_managers(_render_manager);
  command.execute();
}