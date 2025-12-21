#include "base_command.h"

BaseCommand::~BaseCommand() {}

void BaseCommand::set_managers(std::shared_ptr<RenderManager> render_manager) {
  _render_manager = render_manager;
}