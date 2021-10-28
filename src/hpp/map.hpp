#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "entities.hpp"
#include "interactible.hpp"
#include "message.hpp"
#include "search_theta.hpp"
#include "tile.hpp"

namespace crow {
    class game_manager;

struct room {
  // how much to scale the width and length variables of the room when drawing
  // the minimap
  const float minimap_scale = 3.3f;
  unsigned int width = 25;
  unsigned int length = 15;
  unsigned int height = 50;
  bool has_player = false;
  bool has_ai = false;
  float2e minimap_pos = {0.f, 0.f};
  // seconds of oxygen left in the room
  float oxygen = 50;
  // max oxygen in the room
  float oxygen_max = 50;
  int id = 0;
  crow::tile_map tiles;
  crow::theta_star pather;

  float3e cam_pos = float3e(0.f, 20.f, -2.f);
  float3e cam_rotation = float3e(-85.f, 0.f, 0.f);

  // contains the entity index of every object that is in this room
  std::vector<size_t> object_indices;

  // the tile location and type of every object in this room
  // std::unordered_map<glm::uvec2, object_type> room_objects;
  std::vector<crow::interactible*> objects;

  // may not be needed as doors will handle ajacencies
  // std::vector<std::shared_ptr<room>> neighbors;

  void load_entities(game_manager& state);
  void generate_tilemap();

  float2e get_tile_wpos(unsigned int const x, unsigned int const y);
  float2e get_tile_wpos(tile* const tile);
  tile* get_tile_at(float2e const pos);
  std::vector<float2e> get_path(float2e start, float2e goal);
};

// struct door {
//  // the rooms this door is connected to
//  std::vector<std::shared_ptr<room>> rooms;
//
//  void move_entity(const size_t entity_indx, int const room_number);
//};

struct level {
  unsigned int x = 0;
  unsigned int y = 0;
  unsigned int id = 0;
  unsigned int starting_room = 0;
  unsigned int exit_room = 0;
  bool found_ai = false;
  float pressure;
  float pressure_max;
  player_interact* p_inter = nullptr;
  crow::room* selected_room = nullptr;
  std::vector<std::vector<room>> rooms;
  std::vector<door> doors;
  // message to display on the screen. will be read by the game manager to
  // display a message on the top of the screen
  crow::message msg;
  // pointer to object that you are currently interacting with; only used for
  // objects that take time to interact with. do NOT use with anything instant
  crow::interactible* interacting;
  // oxygen console in this floor. if there is no console, or if the console is
  // not broken, then oxygen will not decrease
  crow::oxygen_console* oxygen_console;
  // pressure console in this floor. if there is no console, or if the consle is
  // not broken, then pressure will not decrease (increase?)
  crow::pressure_console* pressure_console;

  // loads a level from file
  void load_level(std::string filepath);

  // loads the hardcoded level with the specified id
  void load_level(crow::game_manager* state, int lv);

  void clean_level();

  // locates the default room and sets it
  // call this when loading a new level AFTER loading entities and BEFORE
  // updating the camera
  void select_default_room();
};

}  // namespace crow