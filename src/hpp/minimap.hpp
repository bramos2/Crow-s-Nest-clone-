#pragma once
#include <liblava/lava.hpp>

#include "../hpp/map.hpp"

namespace crow {

// TODO REMOVE app*
struct item_window {
  lava::app* app;
  // the ratio for the x,y coordinates of the min point of the gui on the screen
  // (top left)
  glm::vec2 screen_minr;
  //  the ratio for the x,y coordinates of the max point of the gui on the
  //  screen (bottom right)
  glm::vec2 screen_maxr;

  glm::vec2 window_pos;
  glm::vec2 window_size;
};

struct minimap {
  lava::app* app;

  // the minimun coordinates of the map
  glm::vec2 map_minc;
  // the maximun coordinates of the map
  glm::vec2 map_maxc;
  // draw scale of objects in the minimap
  glm::vec2 scale;
  // the ratio for the x,y coordinates of the min point of the gui on the screen
  // (top left)
  glm::vec2 screen_minr;
  //  the ratio for the x,y coordinates of the max point of the gui on the
  //  screen (bottom right)
  glm::vec2 screen_maxr;

  glm::vec2 resolution;
  glm::vec2 window_pos;
  glm::vec2 window_ext;

  // x, y, w, h
  // right now, rooms are stored as mere vec4s, but we can make our own struct
  // datatype later on if we need to add more data to it such as connections to
  // other rooms
  std::vector<glm::vec4> rooms;
  std::vector<glm::vec4> blocks;
  // std::vector<map_room*> rooms2;
  // mpos = minimum/maximum position of the minimap space
  // x, y = minimum x, y positions
  // z, w = maximum x, y positions
  // glm::vec4 mpos;
  // cpos = current position of the minimap. this gets added to the position of
  // the rooms in the map when drawing the map
  glm::vec2 cpos;
  // position of the mouse when you click
  // if the mouse hasn't been clicked, this should always be {-1, -1}
  // if the mouse has been clicked, it will be in a range of {0:1, 0:1}
  glm::vec2 mouse_position;
  // returns true if the mouse is dragging rn
  bool dragging;

  void reset_state();

  // checks if the room at index is outside current draw space for minimap
  bool room_off_view(uint_fast16_t i);

  // set size parameters for the minimap window
  void set_window_size(glm::vec2 window_size);

  // checks whether the given mouse position is inside the minimap
  bool inside_minimap(lava::mouse_position_ref mouse_pos);

  void calculate_mouse_position(lava::mouse_position_ref mouse_pos);

  void calculate_mouse_drag(lava::mouse_position_ref mouse_pos);

  void populate_map_data(world_map map);

  minimap();

  minimap(glm::vec2 _min, glm::vec2 _max);

  void draw_minimap();

  void draw_call();
};

struct gui_container {};

}  // namespace crow