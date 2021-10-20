#pragma once
#include <string>
#include <unordered_map>
#include <vector>

#include "../hpp/entities.hpp"
#include "../hpp/interactible.hpp"
#include "../hpp/message.hpp"
#include "../hpp/search_theta.hpp"
#include "../hpp/tile.hpp"

namespace crow {

struct room {
  // how much to scale the width and length variables of the room when drawing
  // the minimap
  const float minimap_scale = 3.3f;
  unsigned int width = 15;
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
  // 0 = floor, 1-4 walls
  // NOTE: must be added to entity system, this is for creation
  std::vector<lava::mesh::ptr> r_meshes;

  float3e cam_pos = float3e(0.f, -7.f, 3.f);
  float3e cam_rotation = float3e(-75.f, 0.f, 0.f);

  // contains the entity index of every object that is in this room
  std::vector<size_t> object_indices;

  // the tile location and type of every object in this room
  // std::unordered_map<glm::uvec2, object_type> room_objects;
  std::vector<crow::interactible*> objects;

  // may not be needed as doors will handle ajacencies
  // std::vector<std::shared_ptr<room>> neighbors;

  void load_entities(lava::app* app, crow::entities& entities,
                     std::vector<lava::mesh::ptr>& meshes,
                     crow::descriptor_writes_stack* writes_stack,
                     lava::descriptor::pool::ptr descriptor_pool,
                     lava::buffer& camera_buffer);

  void make_room_meshes(lava::app* app);

  auto get_tile_wpos(unsigned int const x, unsigned int const y) -> glm::vec2;
  auto get_tile_wpos(tile* const tile) -> glm::vec2;
  auto get_tile_at(glm::vec2 const pos) -> tile*;
  auto get_path(glm::vec2 start, glm::vec2 goal) -> std::vector<glm::vec2>;
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

  void load_entities(lava::app* app, crow::entities& entities,
                     std::vector<lava::mesh::ptr>& meshes,
                     crow::descriptor_writes_stack* writes_stack,
                     lava::descriptor::pool::ptr descriptor_pool,
                     lava::buffer& camera_buffer);

  // loads a level from file
  void load_level(std::string filepath);

  // loads the hardcoded level with the specified id
  void load_level(lava::app* app, int lv);

  // cleans up the currently loaded level, and then proceeds to load the level
  // with the specified id
  // int lv = level to load (NOT the level we are already in)
  void change_level(lava::app* app, int lv);

  void clean_level(std::vector<lava::mesh::ptr>& trash);

  // locates the default room and sets it
  // call this when loading a new level AFTER loading entities and BEFORE
  // updating the camera
  void select_default_room();
};

}  // namespace crow