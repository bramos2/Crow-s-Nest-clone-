#pragma once

#include <liblava/lava.hpp>

namespace crow {
struct minimap {
  // x, y, w, h
  // right now, rooms are stored as mere vec4s, but we can make our own struct
  // datatype later on if we need to add more data to it such as connections to
  // other rooms
  std::vector<ImVec4> rooms;
  // mpos = minimum/maximum position of the minimap.
  // x, y = minimum x, y positions
  // z, w = maximum x, y positions
  ImVec4 mpos;
  // cpos = current position of the minimap. this gets added to the position of
  // the rooms in the map when drawing the map
  ImVec2 cpos;
  // position of the mouse when you click
  // if the mouse hasn't been clicked, this should always be {-1, -1}
  // if the mouse has been clicked, it will be in a range of {0:1, 0:1}
  ImVec2 mouse_position;
  // returns true if the mouse is dragging rn
  bool dragging;
};

void initialize_minimap(minimap& m) {
  m.dragging = 0;
  m.mouse_position = {-1, -1};
}
}  // namespace crow