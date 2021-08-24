#pragma once
#include <liblava/lava.hpp>
#include <cstdint>
#include <vector>

// this file will contain the definition for a block that will represent
// a group of rooms
//template <typename block>
struct map_room {
  // block size and pos
  glm::vec2 center;
  glm::vec2 extent;

  // the rooms this room has a door to
  std::vector<map_room *> neighbors;
  // the position of the top left corner
  glm::vec2 plot_pos;
  // pointer to partent block
  //block *parent_block;
};

struct map_block {
  // block size and pos
  glm::vec2 center;
  glm::vec2 extent;

  // index
  uint_fast16_t x, y;

  // must have a door that leads to a each neighbor
  std::vector<map_block *> neighbors;
  std::vector<std::vector<map_room *>> rooms;

  map_block(uint_fast16_t _x, uint_fast16_t _y);
    
  void generate_rooms(uint_fast8_t const min_rooms, uint_fast8_t const max_rooms,
                      uint_fast8_t const rooms_x, uint_fast8_t const rooms_y,
                      glm::vec2 const block_size, uint_fast8_t const offset);

 private:
  glm::vec2 plot_pos;
};

class world_map {
  // the number of blocks the map will contain
  uint_fast8_t length;
  // max possible blocks in horizontal space
  uint_fast8_t width;
  // max possible blocks in vertical space
  uint_fast8_t height;

  // size of blocks
  glm::vec2 block_size;

  // distance between blocks
  uint_fast8_t offset;

  // constant size of room container inside each block
  uint_fast8_t br_width;
  uint_fast8_t br_height;

  // distance between rooms inside blocks
  uint_fast8_t r_offset;

  // seed used to generate map
  uint_fast32_t seed;

 public:
  // container for blocks
  std::vector<std::vector<map_block *>> blocks;

  void set_block_size(uint_fast16_t w, uint_fast16_t h, uint_fast8_t offset);

  void set_block_space(uint_fast8_t w, uint_fast8_t h,
                       uint_fast8_t room_offset);

  // populates the 2d blocks container
  void generate_map_blockout(glm::vec2 midpoint);

  // returns the smallest and biggest point that represents this map
  // current version actually returns index
  void get_dimensions(glm::vec2 &min, glm::vec2 &max);

  void generate_block_rooms(uint_fast8_t min_rooms, uint_fast8_t max_rooms);

  world_map();
  world_map(uint_fast8_t w, uint_fast8_t h, uint_fast8_t l);
  ~world_map();
};
