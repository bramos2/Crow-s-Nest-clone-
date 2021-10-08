#pragma once
#include <liblava/lava.hpp>

#include "../hpp/map.hpp"

namespace crow {

struct minimap {
  // the minimun coordinates of the map
  glm::vec2 map_minc = {0, 0};
  // the maximun coordinates of the map
  glm::vec2 map_maxc = {0, 0};
  // draw scale of objects in the minimap
  glm::vec2 scale = {0, 0};
  // the ratio for the x,y coordinates of the min point of the gui on the screen
  // (top left)
  glm::vec2 screen_minr = {0, 0};
  //  the ratio for the x,y coordinates of the max point of the gui on the
  //  screen (bottom right)
  glm::vec2 screen_maxr = {0, 0};

  glm::vec2 resolution = {0, 0};
  glm::vec2 window_pos = {0, 0};
  glm::vec2 window_ext = {0, 0};

  crow::level* current_level = nullptr;

  glm::vec2 minimap_center_position = {0, 0};

  // offset at which rooms will be apart from each other in minimap
  float offset = 5.f;
  // the dimmensions at which the room buttons will be desplayed
  float r_width = 45.f;
  float r_height = 45.f;

  void draw_call(lava::app* app);

  minimap();
  minimap(glm::vec2 _min, glm::vec2 _max);
  void set_window_size(glm::vec2 window_size);

 private:
  bool is_dragging = false;

  // position of the mouse when you click
  // if the mouse hasn't been clicked, this = {-1, -1}
  // if the mouse has been clicked, ranges {0:1, 0:1}
  glm::vec2 mouse_position = {0, 0};

  void set_rooms_pos();

  auto inside_minimap(lava::mouse_position_ref& mouse_pos) -> bool;

  void calculate_mouse_position(lava::mouse_position_ref mouse_pos);

  void calculate_mouse_drag(lava::mouse_position_ref mouse_pos);

  auto room_off_view(crow::room const& room) const -> bool;

  void reset_state();
};

}  // namespace crow

//#pragma once
//#include <liblava/lava.hpp>
//
//#include "map.hpp"
//
// namespace crow {
//
//// TODO: REMOVE app*
// struct item_window {
//  // lava::app* app;
//  lava::camera& camera;
//  // the ratio for the x,y coordinates of the min point of the gui on the
//  screen
//  // (top left)
//  glm::vec2 screen_minr;
//  //  the ratio for the x,y coordinates of the max point of the gui on the
//  //  screen (bottom right)
//  glm::vec2 screen_maxr;
//
//  glm::vec2 window_pos;
//  glm::vec2 window_size;
//};
//
// struct minimap {
//  // lava::app* app;
//  lava::camera* camera = nullptr;
//  // the minimun coordinates of the map
//  glm::vec2 map_minc = {0, 0};
//  // the maximun coordinates of the map
//  glm::vec2 map_maxc = {0, 0};
//  // draw scale of objects in the minimap
//  glm::vec2 scale = {0, 0};
//  // the ratio for the x,y coordinates of the min point of the gui on the
//  screen
//  // (top left)
//  glm::vec2 screen_minr = {0, 0};
//  //  the ratio for the x,y coordinates of the max point of the gui on the
//  //  screen (bottom right)
//  glm::vec2 screen_maxr = {0, 0};
//
//  glm::vec2 resolution = {0, 0};
//  glm::vec2 window_pos = {0, 0};
//  glm::vec2 window_ext = {0, 0};
//
//  std::vector<std::shared_ptr<map_room>> room_ptr_list;
//  std::shared_ptr<map_room> active_room;
//  // TODO: Control the block sizes elsewhere.
//  std::vector<crow::map_block<5, 5>>* pblocks_list = nullptr;
//
//  //new version
//  //std::vector<std::shared_ptr<room>> room_ptr_list2;
//  //std::shared_ptr<room> active_room2 = nullptr;
//  //crow::level* current_level = nullptr;
//
//
//  // cpos = current position of the minimap. this gets added to the position
//  of
//  // the rooms in the map when drawing the map
//  glm::vec2 minimap_center_position = {0, 0};
//  // position of the mouse when you click
//  // if the mouse hasn't been clicked, this should always be {-1, -1}
//  // if the mouse has been clicked, it will be in a range of {0:1, 0:1}
//  glm::vec2 mouse_position = {0, 0};
//
//  bool is_dragging = false;
//
//  //void load_room();
//
//  void reset_state();
//
//  // checks if the room at index is outside current draw space for minimap
//  auto room_off_view(std::shared_ptr<crow::map_room>& proom) const -> bool;
//
//  // set size parameters for the minimap window
//  void set_window_size(glm::vec2 window_size);
//
//  // checks whether the given mouse position is inside the minimap
//  auto inside_minimap(lava::mouse_position_ref& mouse_pos) -> bool;
//
//  void calculate_mouse_position(lava::mouse_position_ref mouse_pos);
//
//  void calculate_mouse_drag(lava::mouse_position_ref mouse_pos);
//
//  void populate_map_data(crow::world_map<5, 5>* map, lava::app* app);
//
//  void populate_level_data();
//
//  //void set_level(crow::level& const level, lava::app* app);
//
//  minimap();
//
//  minimap(glm::vec2 _min, glm::vec2 _max);
//
//  void draw_minimap(lava::app* app, lava::mesh::ptr& room_mesh_ptr);
//
//  void draw_call(lava::app* app, lava::mesh::ptr& room_mesh_ptr);
//};
//
// struct gui_container {};
//
//}  // namespace crow
