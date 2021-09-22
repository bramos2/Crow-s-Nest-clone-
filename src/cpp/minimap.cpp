#include "../hpp/minimap.hpp"

#include <imgui.h>

#include "../hpp/camera.hpp"

namespace crow {

void minimap::reset_state() {
  mouse_position = {-1, -1};
  is_dragging = false;
}

auto minimap::room_off_view(std::shared_ptr<crow::map_room>& proom) const
    -> bool {
  // in order, the checks are:
  // 1) check to see if the room is wholly left of the map
  // 2) check to see if the room is wholly above the map
  // 3) check to see if the room is wholly right of the map
  // 4) check to see if the room is wholly under the map
  return (proom->world_x + minimap_center_position.x < -proom->width ||
          proom->world_y + minimap_center_position.y < -proom->height ||
          proom->world_x + proom->width + minimap_center_position.x >
              resolution.x * screen_maxr.x ||
          proom->world_y + proom->height + minimap_center_position.y >
              resolution.y * screen_maxr.y);
}

void minimap::set_window_size(glm::vec2 window_size) {
  window_pos = {window_size.x * screen_minr.x, window_size.y * screen_minr.y};
  window_ext = {window_size.x * screen_maxr.x, window_size.y * screen_maxr.y};
  scale = {window_size.x / resolution.x, window_size.y / resolution.y};
  minimap_center_position = {0, 0};
}

auto minimap::inside_minimap(lava::mouse_position_ref& mouse_pos) -> bool {
  return (mouse_pos.x < window_pos.x + window_ext.x &&
          mouse_pos.y < window_pos.y + window_ext.y &&
          mouse_pos.x > window_pos.x && mouse_pos.y > window_pos.y);
}

void minimap::calculate_mouse_position(lava::mouse_position_ref mouse_pos) {
  mouse_position = {
      static_cast<float>((mouse_pos.x - window_pos.x) / window_ext.x),
      static_cast<float>((mouse_pos.y - window_pos.y) / window_ext.y)};
}

void minimap::calculate_mouse_drag(lava::mouse_position_ref mouse_pos) {
  // position of the mouse on this tick.
  glm::vec2 current_mouse_position = {
      static_cast<float>((mouse_pos.x - window_pos.x) / window_ext.x),
      static_cast<float>((mouse_pos.y - window_pos.y) / window_ext.y)};
  // difference of the position of the mouse on this tick and the last
  // tick
  glm::vec2 mouse_pos_diff = {current_mouse_position.x - mouse_position.x,
                              current_mouse_position.y - mouse_position.y};
  // the mouse position is stored in a range of 0:1 rather than absolute
  // position to have compatibility with the window randomly being resized
  // for no reason at all, so we need to convert it to a range that the
  // minimap understands
  mouse_pos_diff.x *= resolution.x * screen_maxr.x;
  mouse_pos_diff.y *= resolution.y * screen_minr.y;
  // add this difference to the position of the minimap
  minimap_center_position.x += mouse_pos_diff.x;
  minimap_center_position.y += mouse_pos_diff.y;
  // set last frame's position to this frame's position since we are done
  // processing for it
  mouse_position = current_mouse_position;
  // drag boolean predominantly used to ensure that room click processing
  // is not called when releasing the mouse button after dragging the
  // minimap around
  // is_dragging = true;

  // this makes sure that the minimap doesnt get dragged way out of bounds
  // or anything like that
  minimap_center_position.x =
      std::clamp(minimap_center_position.x, map_minc.x, map_maxc.x);
  minimap_center_position.y =
      std::clamp(minimap_center_position.y, map_minc.y, map_maxc.y);
}

void minimap::populate_map_data(crow::world_map<5, 5>* map, lava::app* app) {
  for (auto& block_y : map->block_grid) {
    for (auto& block_x : block_y) {
      if (block_x != nullptr) {
        const auto& block = block_x;
        for (crow::map_room& room : block->room_list) {
          room_ptr_list.push_back(std::make_shared<crow::map_room>(room));
        }
      }
    }
  }
  active_room = room_ptr_list[0];
  active_room->set_active(app, active_room->room_mesh, app->camera);
  // FOR BUILD SHOWCASE PURPOSE ONLY, REMOVE THIS LATER
  for (size_t i = 6; i < 13; i++) {
    active_room->floor_tiles.map[9][i]->is_open = false;
    active_room->floor_tiles.map[i][9]->is_open = false;
  }

  for (size_t i = 0; i < 3; i++) {
    active_room->floor_tiles.map[9][i]->is_open = false;
    active_room->floor_tiles.map[i][9]->is_open = false;
  }

  for (size_t i = 16; i < 19; i++) {
    active_room->floor_tiles.map[9][i]->is_open = false;
    active_room->floor_tiles.map[i][9]->is_open = false;
  }
  // REMOVE AFTER BUILD SHOWCASE
}

minimap::minimap()
    : is_dragging(false),
      mouse_position({-1, -1}),
      screen_minr({0, 0}),
      screen_maxr({0, 0}),
      resolution({0, 0}),
      minimap_center_position({0, 0}) {}

minimap::minimap(glm::vec2 _min, glm::vec2 _max)
    : is_dragging(false),
      screen_minr(_min),
      screen_maxr(_max),
      mouse_position({-1, -1}),
      resolution({0, 0}),
      minimap_center_position({0, 0}) {}

void minimap::draw_call(lava::app* app, lava::mesh::ptr& room_mesh_ptr) {
  draw_minimap(app, room_mesh_ptr);
}

void minimap::draw_minimap(lava::app* app, lava::mesh::ptr& room_mesh_ptr) {
  // pop style var moved into main
  ImGui::SetNextWindowPos((ImVec2&)window_pos, ImGuiCond_Always);
  ImGui::SetNextWindowSize((ImVec2&)window_ext, ImGuiCond_Always);
  // finally create the window
  ImGui::Begin("Facility Map", nullptr,
               ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                   ImGuiWindowFlags_NoTitleBar |
                   ImGuiWindowFlags_NoBringToFrontOnFocus);

  lava::mouse_position_ref _mouse_pos = app->input.get_mouse_position();
  if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
    // first, check to see if the mouse click began inside of the minimap
    // window
    if (inside_minimap(_mouse_pos)) {
      calculate_mouse_position(_mouse_pos);
      fmt::print("i have confirmed the click \n");
    }
  }
  is_dragging = ImGui::IsMouseDragging(ImGuiMouseButton_Left);
  // processing for dragging the minimap around all goes in here
  if (is_dragging) {
    int debug = 1;
    // first check to see that we have "grabbed" the minimap with the mouse.
    // if it has been grabbed, then minimap.mouse_position.x should have been
    // set to a float between 0:1
    if (mouse_position.x != -1) {
      calculate_mouse_drag(_mouse_pos);
    }
  }
  int id = 0;
  for (auto& current_room : room_ptr_list) {
    // Check for out of bounds rooms to skip adding them to draw calls.
    if (room_off_view(current_room)) {
      continue;
    }
    // Set the x, y position of the room:
    ImGui::SetCursorPos({
        static_cast<float>(current_room->world_x) + minimap_center_position.x,
        static_cast<float>(current_room->world_y) + minimap_center_position.y,
    });
    if (ImGui::Button(std::to_string(id++).c_str(),
                      {
                          // Set the width and height:
                          static_cast<float>(current_room->width),
                          static_cast<float>(current_room->height),
                      })) {
      if (!is_dragging) {
        /* processing for room switch goes here */
        active_room = current_room;
        active_room->set_active(app, room_mesh_ptr, *camera);
      }
    }
  }

  // this must be called after all processing for the minimap because it
  // resets variables that may be used while processing the minimap
  if (!ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
    reset_state();
    /*minimap.mouse_position = {-1, -1};
    minimap.dragging = false;*/
  }
  // paranoia check: using !ismousedown instead of mousereleased
  ImGui::End();
}

}  // namespace crow
