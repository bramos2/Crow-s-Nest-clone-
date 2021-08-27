#pragma once
#include "../hpp/minimap.hpp"

#include <imgui.h>

namespace crow {

void minimap::reset_state() {
  mouse_position = {-1, -1};
  dragging = false;
}

bool minimap::room_off_view(uint_fast16_t i) {
  // in order, the checks are:
  // 1) check to see if the room is wholly left of the map
  // 2) check to see if the room is wholly above the map
  // 3) check to see if the room is wholly right of the map
  // 4) check to see if the room is wholly under the map
  return (rooms[i].x + rooms[i].z + cpos.x < 0 ||
          rooms[i].y + rooms[i].w + cpos.y < 0 ||
          rooms[i].x + cpos.x > resolution.x * screen_maxr.x ||
          rooms[i].y + cpos.y > resolution.y * screen_maxr.y);
}

void minimap::set_window_size(glm::vec2 window_size) {
  window_pos = {window_size.x * screen_minr.x, window_size.y * screen_minr.y};
  window_ext = {window_size.x * screen_maxr.x, window_size.y * screen_maxr.y};
  scale = {window_size.x / resolution.x, window_size.y / resolution.y};
  cpos = {0, 0};
}

bool minimap::inside_minimap(lava::mouse_position_ref mouse_pos) {
  return (mouse_pos.x < window_pos.x + window_ext.x &&
          mouse_pos.y < window_pos.y + window_ext.y &&
          mouse_pos.x > window_pos.x && mouse_pos.y > window_pos.y);
}

void minimap::calculate_mouse_position(lava::mouse_position_ref mouse_pos) {
  mouse_position = {(float)((mouse_pos.x - window_pos.x) / window_ext.x),
                    (float)((mouse_pos.y - window_pos.y) / window_ext.y)};
}

void minimap::calculate_mouse_drag(lava::mouse_position_ref mouse_pos) {
  // position of the mouse on this tick.
  glm::vec2 curr_mouse_position = {
      (float)((mouse_pos.x - window_pos.x) / window_ext.x),
      (float)((mouse_pos.y - window_pos.y) / window_ext.y)};
  // difference of the position of the mouse on this tick and the last
  // tick
  glm::vec2 mouse_pos_diff = {curr_mouse_position.x - mouse_position.x,
                              curr_mouse_position.y - mouse_position.y};
  // the mouse position is stored in a range of 0:1 rather than absolute
  // position to have compatibility with the window randomly being resized
  // for no reason at all, so we need to convert it to a range that the
  // minimap understands
  mouse_pos_diff.x *= resolution.x * screen_maxr.x;
  mouse_pos_diff.y *= resolution.y * screen_minr.y;
  // add this difference to the position of the minimap
  cpos.x += mouse_pos_diff.x;
  cpos.y += mouse_pos_diff.y;
  // set last frame's position to this frame's position since we are done
  // processing for it
  mouse_position = curr_mouse_position;
  // drag boolean predominantly used to ensure that room click processing
  // is not called when releasing the mouse button after dragging the
  // minimap around
  dragging = true;

  // this makes sure that the minimap doesnt get dragged way out of bounds
  // or anything like that
  cpos.x = std::clamp(cpos.x, map_minc.x, map_maxc.x);
  cpos.y = std::clamp(cpos.y, map_minc.y, map_maxc.y);
}

void minimap::populate_map_data(world_map& map) {
  for (size_t i = 0; i < map.blocks.size(); i++) {
    for (size_t j = 0; j < map.blocks[i].size(); j++) {
      if (map.blocks[i][j]) {
        map_block* cblock = map.blocks[i][j];
        blocks.push_back({cblock->center.x - cblock->extent.x,
                          cblock->center.y - cblock->extent.y,
                          cblock->extent.x * 2, cblock->extent.y * 2});

        for (size_t y = 0; y < cblock->rooms.size(); y++) {
          for (size_t x = 0; x < cblock->rooms[y].size(); x++) {
            if (cblock->rooms[y][x]) {
              map_room* croom = cblock->rooms[y][x];
              rooms.push_back({croom->plot_pos.x, croom->plot_pos.y,
                               croom->extent.x * 2, croom->extent.y * 2});
            }
          }
        }
      }
    }
  }
}

minimap::minimap()
    : dragging(0),
      mouse_position({-1, -1}),
      screen_minr({0, 0}),
      screen_maxr({0, 0}),
      resolution({0, 0}),
      cpos({0, 0}) {}

minimap::minimap(glm::vec2 _min, glm::vec2 _max)
    : screen_minr(_min),
      screen_maxr(_max),
      dragging(0),
      mouse_position({-1, -1}),
      resolution({0, 0}),
      cpos({0, 0}) {}

void minimap::draw_call() { draw_minimap(); }

void minimap::draw_minimap() {
  // pop style var moved into main
  ImGui::SetNextWindowPos((ImVec2&)window_pos, ImGuiCond_Always);
  ImGui::SetNextWindowSize((ImVec2&)window_ext, ImGuiCond_Always);
  // finally create the window
  ImGui::Begin("Facility Map", 0,
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
      printf("i have confirmed the click \n");
    }
  }

  // processing for dragging the minimap around all goes in here
  if (ImGui::IsMouseDragging(ImGuiMouseButton_Left)) {
    // first check to see that we have "grabbed" the minimap with the mouse.
    // if it has been grabbed, then minimap.mouse_position.x should have been
    // set to a float between 0:1
    if (mouse_position.x != -1) {
      calculate_mouse_drag(_mouse_pos);
    }
  }

  // begin drawing the rooms to imgui, one by one
  for (int i = 0; i < rooms.size(); ++i) {
    // check for out of bounds rooms to skip adding them to draw calls
    if (room_off_view(i)) {
      continue;
    }

    // sets the x, y position of the room
    ImGui::SetCursorPos({(rooms[i].x + cpos.x), (rooms[i].y + cpos.y)});

    // debug stuff, corrupts during run time, wasted 3 hours because of this
    /*char room_number[] = {"##roomX"};
    sprintf(room_number, "##room%i", i);*/

    if (ImGui::Button("r", {rooms[i].z * scale.x, rooms[i].w * scale.y})) {
      if (dragging == false) {
        // printf for testing
        printf("clicked on room %i\n", i);
        /* processing for room switch goes here */
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
