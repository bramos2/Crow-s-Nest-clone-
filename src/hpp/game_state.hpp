#pragma once  // kek

#include <liblava/lava.hpp>

#include <imgui.h>
#include <vector>

#include "../hpp/camera.hpp"
#include "../hpp/enemy_behaviors.hpp"
#include "../hpp/geometry.hpp"
#include "../hpp/map.hpp"
#include "../hpp/minimap.hpp"
#include "../hpp/object.hpp"
#include "../hpp/pipeline.hpp"
#include "../hpp/player_behavior.hpp"

namespace crow {
// a struct that holds overarching game data to help with things such as scene
// switching (main menu, cutscenes, playing, etc)
// any random game related variable should be stored here so it can be accessed
// anywhere
struct game_state {
  // current game state. are we in the main menu? playing? paused? end credits?
  // etc
  enum state : int { MAIN_MENU = 0, PLAYING = 1, PAUSED = 2 } current_state;

  // pointers to important game data so they can be easily accessed
  // crow::descriptor_sets* environment_descriptor_sets;  // to be replaced
  // crow::descriptor_sets* enemy_descriptor_sets;        // to be replaced

  std::vector<crow::descriptor_sets*> desc_sets_list;
  crow::descriptor_writes_stack* descriptor_writes = nullptr;
  crow::minimap minimap;
  crow::entities entities;
  crow::world_map<5, 5> world_map;
  crow::player_behavior_data player_data;
  ai_manager enemy_manager;
  lava::app* app = nullptr;
  lava::buffer camera_buffer;
  crow::camera_device_data camera_buffer_data;
  // ai_manager enemy_manager;

  // time the left click was last pressed
  float left_click_time = 0.f;
  float right_click_time = 0.f;

  // TODO: remove this line
  bool map_created = false;
};

/* all imgui rendering of menus(such as pause menu, options, main menu, etc)
 will go in here

 lava::app& app = reference to the liblava app object, used for callbacks such
 as the close game function lol

 Game_State& game_state = game state object, declared in main and passed through
 by reference very important as it holds a lot of relevant data for drawing the
 menu

 ImVec2& wh = passed in from main imgui rendering function so that it doesnt
 have to be recalculated
 */
void draw_menus(game_state& state, ImVec2 wh);

// start the ENTIRE GAME from the VERY BEGINNING
// should only be called in main() or from the main menu
void new_game(crow::game_state& state);

// cleanup for any game-related objects, called when quitting the game in any
// way, such as clicking the x or trying to start a new game
void end_game(crow::game_state& state);

void clean_state(crow::game_state& state);

void update(crow::game_state& state, lava::delta dt);

auto left_click_update(game_state& state) -> bool;

auto right_click_update(game_state& state) -> bool;

}  // namespace crow