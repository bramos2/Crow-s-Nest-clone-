#include "../hpp/minimap.hpp"
#include "../hpp/game_manager.hpp"

void crow::minimap::draw_call(game_manager& state) {
    ImGui::SetNextWindowBgAlpha(0.3f);

    // special processing depending on if the mouse is inside the minimap
    if (inside_minimap(state.mouse_pos_gui) || is_dragging) {
        // first, enable zooming of the map
        if (state.mwheel_delta) {
            calculate_extents(false);
            zoom += state.mwheel_delta * 0.001f;
            zoom = clampf(zoom, 1, 5);
            // TODO:: support for shift + rclick
        }

        // next, draw the minimap not faded out
        ImGui::SetNextWindowBgAlpha(0.55f);
    }

    ImGui::SetNextWindowPos((ImVec2&)window_pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize((ImVec2&)window_ext, ImGuiCond_Always);
    // finally create the window
    ImGui::Begin("Facility Map", nullptr,
                ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                    ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoScrollbar |
                    ImGuiWindowFlags_NoTitleBar |
                    ImGuiWindowFlags_NoBringToFrontOnFocus);

    // keeps the minimap properly sized for the game window
    ImVec2 wh = state.imgui_wsize;
    set_window_size(wh);

  if (ImGui::IsMouseClicked(ImGuiMouseButton_Right)) {
    // first, check to see if the mouse click began inside of the minimap
    // window
    if (inside_minimap(state.mouse_pos_gui)) {
      calculate_mouse_position(state.mouse_pos_gui);
    }
  }
    // this is used to prevent buttons from being clicked when you drag on the minimap
    bool was_dragging = is_dragging;

    is_dragging = ImGui::IsMouseDragging(ImGuiMouseButton_Right);
    // processing for dragging the minimap around all goes in here
    if (is_dragging) {
        // first check to see that we have "grabbed" the minimap with the mouse.
        // if it has been grabbed, then minimap.mouse_position.x should have been
        // set to a float between 0:1
        if (mouse_position.x != -1) {
            calculate_mouse_drag(state.mouse_pos_gui);
        }
        was_dragging = true;
    }
    // colors of the imgui minimap buttons (default)
        ImGui::PushStyleColor(ImGuiCol_Button, { 0.19f, 0.40f, 0.63f, 0.35f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.25f, 0.58f, 0.98f, 0.7f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.25f, 0.58f, 0.98f, 0.7f});

  // float2e starting_r_pos = {0.f, 0.f};
  if (current_level) {
    for (auto& current_row : current_level->rooms) {
      for (auto& current_room : current_row) {
        // Check for out of bounds rooms to skip adding them to draw calls.
        if (current_room.id == 0 || room_off_view(current_room)) {
          continue;
        }

        // this readjusts the room for the screen
        ImVec2 room_xy = {
            (static_cast<float>(current_room.minimap_pos.x) +
             minimap_center_position.x) *
                scale.x,
            (static_cast<float>(current_room.minimap_pos.y) +
             minimap_center_position.y) *
                scale.y,
        };
        ImVec2 room_wh = {
            // Set the width and height:
            ((current_room.length * current_room.minimap_scale)) * scale.x,
            ((current_room.length * current_room.minimap_scale)) * scale.y,
            // room tile size is length x length instead of length x width
            // so they are the same size even though the rooms are not the same size
        };

        float thickness = 1.f;
        if (&current_room == current_level->selected_room) thickness = 5.f;
        
        // proof of concept room outlining
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(window_pos.x + room_xy.x, window_pos.y + room_xy.y),
            ImVec2(window_pos.x + room_xy.x + room_wh.x,
                   window_pos.y + room_xy.y),
            IM_COL32(255, 255, 255, 255), thickness);
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(window_pos.x + room_xy.x, window_pos.y + room_xy.y),
            ImVec2(window_pos.x + room_xy.x,
                   window_pos.y + room_xy.y + room_wh.y),
            IM_COL32(255, 255, 255, 255), thickness);
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(window_pos.x + room_xy.x + room_wh.x,
                   window_pos.y + room_xy.y + room_wh.y),
            ImVec2(window_pos.x + room_xy.x + room_wh.x,
                   window_pos.y + room_xy.y),
            IM_COL32(255, 255, 255, 255), thickness);
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(window_pos.x + room_xy.x + room_wh.x,
                   window_pos.y + room_xy.y + room_wh.y),
            ImVec2(window_pos.x + room_xy.x,
                   room_xy.y + window_pos.y + room_wh.y),
            IM_COL32(255, 255, 255, 255), thickness);
        //

        // Set the x, y position of the room:
        ImGui::SetCursorPos(room_xy);

        // change the color of the room button where the player is
        if (current_room.has_player) {
            ImGui::PopStyleColor(3);
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.65f, 0.25f, 0.25f, 0.35f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.85f, 0.37f, 0.37f, 0.7f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.85f, 0.37f, 0.37f, 0.7f});
        }

        //if (ImGui::Button((state.debug_mode ? std::to_string(current_room.id).c_str() : std::string("##") + std::to_string(current_room.id)).c_str(), room_wh)) {
        if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && !was_dragging && inside_minimap(state.mouse_pos_gui) &&
            inside_pos(state.mouse_pos_gui, room_xy, room_wh)) {
            /* processing for room switch goes here */
            // active_room->set_active(app, room_mesh_ptr, *camera);
            printf("\nclicked on room: ");
            printf(std::to_string(current_room.id).c_str());
            current_level->selected_room = &current_room;
            //crow::update_room_cam(current_level->selected_room, state.view);
            crow::update_room_cam(pac(state.cam_pos), pac(state.cam_rotation), state.view);
            state.change_room_tex();
        }
        ImGui::Button((state.debug_mode ? std::to_string(current_room.id).c_str() : std::string("##") + std::to_string(current_room.id)).c_str(), room_wh);
        
        
        // reset colors
        if (current_room.has_player) {
            ImGui::PopStyleColor(3);
            ImGui::PushStyleColor(ImGuiCol_Button, { 0.19f, 0.40f, 0.63f, 0.35f});
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.25f, 0.58f, 0.98f, 0.7f});
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.25f, 0.58f, 0.98f, 0.7f});
        }

      }
      // starting_r_pos.y += r_height + offset;
      // this must be called after all processing for the minimap because it
      // resets variables that may be used while processing the minimap

      // paranoia check: using !ismousedown instead of mousereleased
    }
  }
  if (!ImGui::IsMouseDown(ImGuiMouseButton_Right)) {
    reset_state();
    /*minimap.mouse_position = {-1, -1};
    minimap.dragging = false;*/
  }
    ImGui::PopStyleColor(3);
  ImGui::End();

  
}

crow::minimap::minimap() {}

crow::minimap::minimap(float2e _min, float2e _max)
    : is_dragging(false),
      screen_minr(_min),
      screen_maxr(_max),
      mouse_position({-1, -1}),
      resolution({0, 0}),
      minimap_center_position({0, 0}) {}

void crow::minimap::set_window_size(ImVec2 window_size) {
  window_pos = {window_size.x * screen_minr.x, window_size.y * screen_minr.y};
  window_ext = {window_size.x * screen_maxr.x, window_size.y * screen_maxr.y};
  scale = {window_size.x / resolution.x * zoom,
           window_size.y / resolution.y * zoom};
  // minimap_center_position = {0, 0};
}

void crow::minimap::set_rooms_pos() {
  if (current_level) {
    float2e starting_position = {0.f, 0.f};
    for (auto& current_row : current_level->rooms) {
      starting_position.x = 0.f;
      for (auto& current_room : current_row) {
        current_room.minimap_pos = starting_position;
        starting_position.x += current_room.length * current_room.minimap_scale + offset;
      }
      starting_position.y += 49.5 + offset;
    }
  }
}

auto crow::minimap::inside_minimap(float2e& mouse_pos)
    -> bool {
  return (mouse_pos.x < static_cast<double>(window_pos.x) + static_cast<double>(window_ext.x) &&
          mouse_pos.y < static_cast<double>(window_pos.y) + static_cast<double>(window_ext.y) &&
          mouse_pos.x > static_cast<double>(window_pos.x) &&
            static_cast<double>(mouse_pos.y) > static_cast<double>(window_pos.y));
}

auto crow::minimap::inside_pos(float2e& mouse_pos, ImVec2 pos, ImVec2 wh)
    -> bool {
  return (mouse_pos.x < static_cast<double>(window_pos.x) + static_cast<double>(pos.x) + static_cast<double>(wh.x) &&
          mouse_pos.y < static_cast<double>(window_pos.y) + static_cast<double>(pos.y) + static_cast<double>(wh.y) &&
          mouse_pos.x > static_cast<double>(window_pos.x) + static_cast<double>(pos.x) &&
            static_cast<double>(mouse_pos.y) > static_cast<double>(window_pos.y) + static_cast<double>(pos.y));
}

void crow::minimap::calculate_mouse_position(float2e& mouse_pos) {
  mouse_position = {
      static_cast<float>((mouse_pos.x - window_pos.x) / window_ext.x),
      static_cast<float>((mouse_pos.y - window_pos.y) / window_ext.y)};
}

void crow::minimap::calculate_mouse_drag(float2e& mouse_pos) {  // position of the mouse on this tick.
  float2e current_mouse_position = {
      static_cast<float>((mouse_pos.x - window_pos.x) / window_ext.x),
      static_cast<float>((mouse_pos.y - window_pos.y) / window_ext.y)};
  // difference of the position of the mouse on this tick and the last
  // tick
  float2e mouse_pos_diff = {current_mouse_position.x - mouse_position.x,
                              current_mouse_position.y - mouse_position.y};
  // the mouse position is stored in a range of 0:1 rather than absolute
  // position to have compatibility with the window randomly being resized
  // for no reason at all, so we need to convert it to a range that the
  // minimap understands
  mouse_pos_diff.x *= resolution.x * screen_maxr.x;
  mouse_pos_diff.y *= resolution.y * screen_maxr.y;
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
  minimap_center_position.x = clampf(minimap_center_position.x, map_minc.x, map_maxc.x);
  minimap_center_position.y = clampf(minimap_center_position.y, map_minc.y, map_maxc.y);
}

void crow::minimap::calculate_extents(bool recenter) {
  set_rooms_pos();

  float2e min = {0, 0};
  float2e max = {0, 0};

  int rooms_checked = 0;
  for (auto& current_row : current_level->rooms) {
    for (auto& current_room : current_row) {
      if (current_room.id == 0) continue;
      if (rooms_checked) {
        // check to see if this room is further out than the other rooms we have
        // already checked
        if (current_room.minimap_pos.x < min.x) min.x = current_room.minimap_pos.x;
        if (current_room.minimap_pos.y < min.y) min.y = current_room.minimap_pos.y;
        if (current_room.minimap_pos.x + current_room.length * current_room.minimap_scale > max.x) max.x = current_room.minimap_pos.x + current_room.length * current_room.minimap_scale;
        if (current_room.minimap_pos.y + current_room.length * current_room.minimap_scale > max.y) max.y = current_room.minimap_pos.y + current_room.length * current_room.minimap_scale;
        // clang-format on
      } else {
        // this is the first room in the list, so it's the standard to check all
        // other rooms to (we need SOMETHING as a basic to check with)
        min = float2e(current_room.minimap_pos.x, current_room.minimap_pos.y);
        max = float2e(current_room.minimap_pos.x + current_room.length * current_room.minimap_scale,
                        current_room.minimap_pos.y + current_room.length * current_room.minimap_scale);
      }
      rooms_checked++;
    }
  }

  // correctly set the extents based on the size of the map that we just
  // calculated
  map_minc = {-max.x, -max.y};
  map_maxc = {-min.x, -min.y};

  // don't ask why any of this works. it just does :P
  float2e diff = {max.x - min.x, max.y - min.y};
  float2e ext = {(screen_maxr.x) * resolution.x,
                   (screen_maxr.y) * resolution.y};
  map_maxc.x += ext.x / zoom;
  map_maxc.y += ext.y / zoom;

  // essentially a "how much of the map do you want to allow offscreen?"
  // variable
  // going below 0 is a great way to make the game crash. 0 itself
  // comically locks the map in place
  float dist = 0.35f;
  float2e mid = {(map_maxc.x + map_minc.x) / 2,
                   (map_maxc.y + map_minc.y) / 2};
  map_minc = {mid.x - diff.x * dist, mid.y - diff.y * dist};
  map_maxc = {mid.x + diff.x * dist, mid.y + diff.y * dist};
  // possible todo: the map looks great at 0.2f, but values lower than 0.5f
  // cause the map to grow larger than the viewable area when a combination of
  // zoom and map size grows too big this may never be fixed if we end up never
  // making a map big enough for this

    if (recenter) {
        minimap_center_position = mid;
    } else {
        // if we're not going to center the map, at least prevent it from going out of bounds
        minimap_center_position.x = clampf(minimap_center_position.x, map_minc.x, map_maxc.x);
        minimap_center_position.y = clampf(minimap_center_position.y, map_minc.y, map_maxc.y);
    }
}

auto crow::minimap::room_off_view(crow::room const& room) const -> bool {
    float2e wh = {room.width * room.minimap_scale, room.length * room.minimap_scale};

  return (room.minimap_pos.x + minimap_center_position.x < -wh.y ||
          room.minimap_pos.y + minimap_center_position.y < -wh.y ||
          room.minimap_pos.x + wh.y + minimap_center_position.x >
              resolution.x * screen_maxr.x ||
          room.minimap_pos.y + wh.y + minimap_center_position.y >
              resolution.y * screen_maxr.y);
}
void crow::minimap::reset_state() {
  mouse_position = {-1, -1};
  is_dragging = false;
}