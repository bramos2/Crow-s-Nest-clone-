#pragma once // kek

#include <liblava/lava.hpp>

#include <imgui.h>

#include "audio.hpp"
#include "geometry.hpp"
#include "map.hpp"
#include "minimap.hpp"
#include "object.hpp"
#include "pipeline.hpp"

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
  crow::descriptor_sets* environment_descriptor_sets;
  crow::descriptor_writes_stack* descriptor_writes;
  crow::minimap* minimap;
  crow::entities* entities;
  crow::world_map<5, 5> world_map;
  lava::app* app;

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

}  // namespace crow