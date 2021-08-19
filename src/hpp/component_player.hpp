#pragma once

//#include "component.hpp"

namespace crow {
struct Component_Player : Component {
  // represents the current position of the mouse pointer on the screen
  lava::v2 mouse_position;
  // represents the current position of the mouse pointer as extrapolated to the
  // 3d world quite simply, it means the point in space that has been clicked on
  lava::v3 clicked_position;

  const int component_type = 1;

  void update(const lava::app* const _app, std::vector<Object>* _objects);
  inline Component_Type get_type() const { return Component_Type::PLAYER; }

  // left clicked on the screen
  void left_click(const lava::app* const _app, std::vector<Object>* _objects);
};

}  // namespace crow
