#pragma once
#include <liblava/lava.hpp>

#include <string>
#include <unordered_map>
#include <vector>

#include "../hpp/entities.hpp"
#include "../hpp/game_objects.hpp"
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
  glm::vec2 minimap_pos = {0.f, 0.f};
  int id = 0;
  crow::tile_map tiles;
  crow::theta_star pather;
  // 0 = floor, 1-4 walls
  // NOTE: must be added to entity system, this is for creation
  std::vector<lava::mesh::ptr> r_meshes;

  lava::v3 cam_pos = lava::v3(0.f, -7.f, 3.f);
  lava::v3 cam_rotation = lava::v3(-75.f, 0.f, 0.f);

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
  crow::room* selected_room = nullptr;
  std::vector<std::vector<room>> rooms;
  std::vector<door> doors;
  // message to display on the screen. will be read by the game manager to
  // display a message on the top of the screen
  crow::message msg;
  // pointer to object that you are currently interacting with; only used for
  // objects that take time to interact with. do NOT use with anything instant
  crow::interactible* interacting;

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

//#pragma once
//#include <liblava/lava.hpp>
//
//#include <algorithm>
//#include <cassert>
//#include <cstdlib>
//#include <ctime>
//#include <limits>
//#include <memory>
//#include <optional>
//#include <vector>
//
//#include "../hpp/collision.hpp"
//#include "../hpp/interactible.hpp"
//#include "../hpp/search_theta.hpp"
//#include "../hpp/tile.hpp"
//
// namespace crow {
//
//#define WORLD_SEED std::time(NULL)
//#define DEBUG_SEED 9
// constexpr int block_world_width = 80;
// constexpr int block_world_height = 80;
// constexpr int block_world_padding = 5;
// constexpr int room_world_padding = 5;
//
// struct map_room {
//  int const world_x, world_y, width, height;
//  std::vector<std::shared_ptr<crow::map_room>> neighbors;
//  crow::collision_universe collision_universe;
//  // the position and rotation of the camera
//  glm::vec3 cam_pos = {0, -7, 3};
//  glm::vec3 cam_rotation = {-75.0f, 0, 0.0f};
//  lava::mesh_data room_mesh_data;
//  lava::mesh::ptr room_mesh;
//  crow::tile_map floor_tiles;
//  // list of items in this room
//  std::vector<interactible*> consoles;
//  // std::vector<item> items;
//  // rooms can have at most 1 self destruct console
//  // sd_console* r_console = nullptr;
//  void set_active(lava::app* app, lava::mesh::ptr& mesh_ptr,
//                  lava::camera& camera);
//  glm::vec2 get_tile_wpos(unsigned int const x, unsigned int const y);
//  glm::vec2 get_tile_wpos(tile* const tile);
//  tile* get_tile_at(glm::vec2 const pos);
//  std::vector<glm::vec2> get_path(glm::vec2 start, glm::vec2 goal);
//};
//
// template <int br_width, int br_height>
// struct map_block {
//  // TODO: Make x and y const.
//  int block_index_x, block_index_y;
//  std::vector<std::shared_ptr<crow::map_block<br_width, br_height>>>
//  neighbors; std::vector<crow::map_room> room_list; void generate_rooms(int
//  min_rooms, int max_rooms);
//};
//
// template <int br_width, int br_height>
// struct world_map {
//  std::array<std::array<std::shared_ptr<crow::map_block<br_width, br_height>>,
//                        br_width>,
//             br_height>
//      block_grid;
//  void generate_blocks(int blocks_count);
//  void generate_rooms(int min_rooms, int max_rooms);
//  void generate_adjacencies();
//};
//
// template <int br_width, int br_height>
// void world_map<br_width, br_height>::generate_blocks(int blocks_count) {
//  // center position of our map
//  int const starting_index_x = br_width / 2;
//  int const starting_index_y = br_height / 2;
//  crow::map_block<br_width, br_height> starting_block;
//  int current_index_x = starting_index_x;
//  int current_index_y = starting_index_y;
//  std::srand(DEBUG_SEED);
//
//  // generating blockout
//  int current_blocks_count = 0;
//  while (current_blocks_count < blocks_count) {
//    unsigned random_number = std::floor(rand() % 4);
//    // determining the direction we are moving to in this frame based on
//    random switch (random_number) {
//      case 0: {
//        ++current_index_x;
//        break;
//      }
//      case 1: {
//        ++current_index_y;
//        break;
//      }
//      case 2: {
//        --current_index_x;
//        break;
//      }
//      default: {
//        --current_index_y;
//        break;
//      }
//    }
//
//    // Reset position if the current position is out of bounds.
//    [[unlikely]] if (current_index_x < 0 || current_index_x > br_width ||
//                     current_index_y < 0 || current_index_y > br_height) {
//      current_index_x = starting_index_x;
//      current_index_y = starting_index_y;
//    }
//    // Check if the current position is occupied by another block.
//    if (block_grid[current_index_y][current_index_x] != nullptr) {
//      continue;
//    }
//
//    // Create a new block here.
//    // crow::map_block<br_width, br_height> curr;
//    auto curr = std::make_shared<crow::map_block<br_width, br_height>>();
//    curr->block_index_x = current_index_x;
//    curr->block_index_y = current_index_y;
//    block_grid[current_index_y][current_index_x] = curr;
//    current_blocks_count++;
//  }
//
//  // connecting all blocks with their neighbors
//  for (ptrdiff_t i = 0; i < br_width; i++) {
//    for (ptrdiff_t j = 0; j < br_height; j++) {
//      if (block_grid[i][j] != nullptr) {
//        auto current_block = block_grid[i][j];
//        if (i > 0) {
//          if (block_grid[i - 1][j] != nullptr) {
//            auto unwrapped_block = block_grid[i - 1][j];
//            current_block->neighbors.push_back(unwrapped_block);
//          }
//        }
//        if (j > 0) {
//          if (block_grid[i][j - 1] != nullptr) {
//            auto unwrapped_block = block_grid[i][j - 1];
//            current_block->neighbors.push_back(unwrapped_block);
//          }
//        }
//        if (i < br_width - 1) {
//          if (block_grid[i + 1][j] != nullptr) {
//            auto unwrapped_block = block_grid[i + 1][j];
//            current_block->neighbors.push_back(unwrapped_block);
//          }
//        }
//        if (j < br_height - 1) {
//          if (block_grid[i][j + 1] != nullptr) {
//            auto unwrapped_block = block_grid[i][j + 1];
//            current_block->neighbors.push_back(unwrapped_block);
//          }
//        }
//      }
//    }
//  }
//}
//
// template <int br_width, int br_height>
// void map_block<br_width, br_height>::generate_rooms(int min_rooms,
//                                                    int max_rooms) {
//  auto overlaps_room = [&](std::vector<crow::map_room>& rooms,
//                           int current_world_x, int current_world_y,
//                           int current_room_width,
//                           int current_room_height) -> bool {
//    for (const crow::map_room& room : rooms) {
//      if (
//          // If the other room is rightward of the current room's left
//          boundary: room.world_x + room.width > current_world_x &&
//          // If the other room is leftward of
//          // the current room's right boundary:
//          room.world_x < current_world_x + current_room_width &&
//          // If the other room is below of the current room's top
//          // boundary:
//          room.world_y + room.height > current_world_y &&
//          // If the other room is above of the
//          // current room's bottom boundary:
//          room.world_y < current_world_y + current_room_height) {
//        return true;
//      }
//    }
//    return false;
//  };
//
//  // Generate either the minimum number of possible rooms or some random index
//  // that is higher.
//  int random_rooms_count = std::max<int>(rand() % max_rooms, min_rooms);
//  for (int i = 0; i < random_rooms_count; i++) {
//    // TODO: These should be read from a room prefab pool.
//    int room_width = rand() % 10 + 10;
//    int room_height = rand() % 10 + 10;
//    int x = crow::block_world_width / 2;
//    int y = crow::block_world_height / 2;
//    // Random walk this room.
//    while (overlaps_room(this->room_list,
//                         x + this->block_index_x * crow::block_world_width -
//                             crow::room_world_padding,
//                         y + this->block_index_y * crow::block_world_height -
//                             crow::room_world_padding,
//                         room_width + crow::room_world_padding * 2,
//                         room_height + crow::room_world_padding * 2)) {
//      // Increment x and y in by (-1 | 0 | 1)
//      x += (rand() % 3) - 1;
//      y += (rand() % 3) - 1;
//      // Keep this room within bounds.
//      [[unlikely]] if (x + room_width > crow::block_world_width || x < 0 ||
//                       y + room_height > crow::block_world_height || y < 0) {
//        x = crow::block_world_width / 2;
//        y = crow::block_world_height / 2;
//      }
//    }
//    this->room_list.push_back(crow::map_room{
//        .world_x = x + this->block_index_x * crow::block_world_width,
//        .world_y = y + this->block_index_y * crow::block_world_height,
//        .width = room_width,
//        .height = room_height,
//    });
//    // default room mesh data being loaded
//    map_room* curr_room = &room_list.back();
//    curr_room->cam_pos;
//    curr_room->room_mesh_data = lava::create_mesh_data(lava::mesh_type::cube);
//
//    // room_list.back().room_mesh_data.move({0, 0.0f, 0});
//    curr_room->room_mesh_data.scale_vector(
//        {static_cast<float>(room_width * 2), 0.2f,
//         static_cast<float>(room_height * 2)});
//    curr_room->floor_tiles = crow::tile_map(room_width, room_height);
//    curr_room->floor_tiles.min_room_pos =
//        glm::vec2(-room_width / 2.0f, -room_height / 2.0f);
//    curr_room->floor_tiles.max_room_pos =
//        glm::vec2(room_width / 2.0f, room_height / 2.0f);
//    curr_room->floor_tiles.create_map();
//  }
//
//  // temporary for mesh creation
//  /*for (size_t i = 0; i < room_list.size(); i++) {
//    room_list[i].room_mesh_data =
//    lava::create_mesh_data(lava::mesh_type::cube);
//    room_list[i].room_mesh_data.move({0, 10.0f, 0});
//    room_list[i].room_mesh_data.scale_vector({10.0f, 0.2f, 10.0f});
//  }*/
//}
//
// template <int br_width, int br_height>
// void world_map<br_width, br_height>::generate_rooms(int min_rooms,
//                                                    int max_rooms) {
//  for (auto& block_x : block_grid) {
//    for (auto& block : block_x) {
//      if (block != nullptr) {
//        auto unwrapped_block = block;
//        unwrapped_block->generate_rooms(min_rooms, max_rooms);
//      }
//    }
//  }
//}
//
// template <int br_width, int br_height>
// void world_map<br_width, br_height>::generate_adjacencies() {
//  // TODO: Generate flat rooms ahead of time.
//  std::vector<std::shared_ptr<crow::map_room>> flat_rooms;
//  for (auto& block_y : this->block_grid) {
//    for (auto& block_x : block_y) {
//      if (block_x != nullptr) {
//        for (crow::map_room& room : block_x->room_list) {
//          auto shared_room = std::make_shared<crow::map_room>(room);
//          flat_rooms.push_back(shared_room);
//        }
//      }
//    }
//  }
//  for (auto& current_room : flat_rooms) {
//    for (auto& other_room : flat_rooms) {
//      if (&current_room == &other_room) {
//        continue;
//      }
//      int x = (other_room->world_x + other_room->width / 2) -
//              (current_room->world_x + current_room->width / 2);
//      int y = (other_room->world_y + other_room->height / 2) -
//              (current_room->world_y + current_room->height / 2);
//      if (x * x + y * y < 40 * 40) {
//        const std::shared_ptr<map_room>& pneighbor = other_room;
//        current_room->neighbors.push_back(pneighbor);
//      }
//    }
//  }
//}
//
//}  // namespace crow
