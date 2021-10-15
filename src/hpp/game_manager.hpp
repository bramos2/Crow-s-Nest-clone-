#pragma once
#include <liblava/lava.hpp>

#include <imgui.h>
#include <vector>
#include <time.h>

#include "../hpp/camera.hpp"
#include "../hpp/entities.hpp"
#include "../hpp/geometry.hpp"
#include "../hpp/minimap.hpp"
#include "../hpp/player_behavior.hpp"
#include "../hpp/behavior_tree.hpp"

namespace crow {

class game_manager {
 public:
  enum class game_state {
    MAIN_MENU = 0,
    PLAYING = 1,
    PAUSED = 2,
    SETTINGS = 3,
    CREDITS = 4,
    LOADING = 5,
    EXIT = 6,
    GAME_OVER,
    GAME_OVER_PRE
  } current_state = game_state::MAIN_MENU;

  game_state prev_state = game_state::MAIN_MENU;
  float state_time = 0;

  int argc = -1;
  char** argv = nullptr;
  lava::frame_config config;
  lava::app* app = nullptr;

  lava::buffer camera_buffer;
  crow::camera_device_data camera_buffer_data = {glm::identity<lava::mat4>()};
   
  lava::mat4 world_matrix_buffer_data = glm::identity<lava::mat4>();
  lava::buffer world_matrix_buffer;

  lava::descriptor::pool::ptr descriptor_pool;

  lava::graphics_pipeline::ptr environment_pipeline;
  lava::pipeline_layout::ptr environment_pipeline_layout;

  crow::descriptor_layouts environment_descriptor_layouts;
  crow::descriptor_sets environment_descriptor_sets = {};
  crow::descriptor_writes_stack descriptor_writes;

  // this container will have a mesh of every model tht will be used in game
  std::vector<lava::mesh::ptr> mesh_models;

  // since destroy crashes program just move the pointers somewhere else
  // TODO: Find a way to clean out the ptrs without consuming more and more
  // memory
  std::vector<lava::mesh::ptr> mesh_models_trash;

  crow::entities entities;

  crow::minimap minimap;

  crow::level test_level;

  crow::player_behavior_data player_data;

  crow::behavior_tree ai_bt;
  crow::ai_manager ai_m;

  float left_click_time = 0.f;
  float right_click_time = 0.f;
  int menu_position = 0;

  game_manager();
  game_manager(int _argc, char* _argv[]);
  ~game_manager();

  void init_app();

 private:
  // app functions
  auto on_create() -> lava::app::create_func;
  auto on_destroy() -> lava::app::destroy_func;
  auto on_update() -> lava::app::update_func;
  auto imgui_on_draw() -> lava::imgui::draw_func;
  auto mouse_events() -> lava::mouse_button_event::func;
  auto run_end() -> lava::frame::run_end_func_ref;

  // game state data functions
  void new_game();
  void load_mesh_data();
  void unload_game();
  void render_game();
  auto l_click_update() -> bool;
  auto r_click_update() -> bool;
  void cleanup();

  // gui functions
  void draw_main_menu();
  void draw_pause_button();
  void draw_pause_menu();
  void draw_control_message();
  void draw_game_over();

  // helper functions

  // draws some imgui text in the center of the current window
  void imgui_centertext(std::string text, float scale, ImVec2 wh);
};

}  // namespace crow

//#pragma once  // kek
//
//#include <liblava/lava.hpp>
//
//#include <imgui.h>
//#include <vector>
//
//#include "../hpp/camera.hpp"
//#include "../hpp/enemy_behaviors.hpp"
//#include "../hpp/geometry.hpp"
//#include "../hpp/map.hpp"
//#include "../hpp/minimap.hpp"
//#include "../hpp/object.hpp"
//#include "../hpp/pipeline.hpp"
//#include "../hpp/player_behavior.hpp"
//
// namespace crow {
//// a struct that holds overarching game data to help with things such as scene
//// switching (main menu, cutscenes, playing, etc)
//// any random game related variable should be stored here so it can be
/// accessed / anywhere
// struct game_state {
//  // current game state. are we in the main menu? playing? paused? end
//  credits?
//  // etc
//  enum state : int { MAIN_MENU = 0, PLAYING = 1, PAUSED = 2 } current_state;
//
//  // pointers to important game data so they can be easily accessed
//  // crow::descriptor_sets* environment_descriptor_sets;  // to be replaced
//  // crow::descriptor_sets* enemy_descriptor_sets;        // to be replaced
//
//  std::vector<crow::descriptor_sets> desc_sets_list;
//  crow::descriptor_writes_stack* descriptor_writes = nullptr;
//  crow::minimap minimap;
//  crow::entities entities;
//  crow::world_map<5, 5> world_map;
//  crow::player_behavior_data player_data;
//  ai_manager enemy_manager;
//  lava::app* app = nullptr;
//  lava::buffer camera_buffer;
//  crow::camera_device_data camera_buffer_data;
//  std::shared_ptr<crow::map_room> active_room;
//
//  // simple variables to check for win condition
//  unsigned int win_condition = 0;
//  const unsigned int win_goal = 1;
//  // ai_manager enemy_manager;
//
//  // time the left click was last pressed
//  float left_click_time = 0.f;
//  float right_click_time = 0.f;
//
//  // TODO: remove this line
//  bool map_created = false;
//};
//
///* all imgui rendering of menus(such as pause menu, options, main menu, etc)
// will go in here
//
// lava::app& app = reference to the liblava app object, used for callbacks such
// as the close game function lol
//
// Game_State& game_state = game state object, declared in main and passed
// through by reference very important as it holds a lot of relevant data for
// drawing the menu
//
// ImVec2& wh = passed in from main imgui rendering function so that it doesnt
// have to be recalculated
// */
// void draw_menus(game_state& state, ImVec2 wh);
//
//// start the ENTIRE GAME from the VERY BEGINNING
//// should only be called in main() or from the main menu
// void new_game(crow::game_state& state);
//
//// cleanup for any game-related objects, called when quitting the game in any
//// way, such as clicking the x or trying to start a new game
// void end_game(crow::game_state& state);
//
// void clean_state(crow::game_state& state);
//
// void update(crow::game_state& state, lava::delta dt);
//
// auto left_click_update(game_state& state) -> bool;
//
// auto right_click_update(game_state& state) -> bool;
//}  // namespace crow