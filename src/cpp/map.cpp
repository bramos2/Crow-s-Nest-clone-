#pragma once
#include "../hpp/map.hpp"

#include <assert.h>
#include <limits>
#include <time.h>

map_block::map_block(uint_fast16_t _x, uint_fast16_t _y)
    : x(_x), y(_y), center(0), extent(0), plot_pos(0) {}

void map_block::generate_rooms(uint_fast8_t const min_rooms,
                               uint_fast8_t const max_rooms,
                               uint_fast8_t const rooms_x,
                               uint_fast8_t const rooms_y,
                               glm::vec2 const block_size,
                               uint_fast8_t const offset) {
  // starting index
  const uint_fast8_t sx = (rooms_x % 2 == 0) ? floor((rooms_x - 1) / 2 + 0.5f)
                                             : floor(rooms_x / 2 + 0.5f);
  const uint_fast8_t sy = (rooms_y % 2 == 0) ? floor((rooms_y - 1) / 2 + 0.5f)
                                             : floor(rooms_y / 2 + 0.5f);

  // number of rooms this block will contain
  const uint_fast8_t n = rand() % (max_rooms - min_rooms + 1) + min_rooms;

  // reserving space for rooms
  rooms.resize(rooms_y);
  for (auto& row : rooms) {
    row.resize(rooms_x);
  }

  // the size in map space of the rooms based on the size of the block itself
  const float r_width = (block_size.x - (offset * (rooms_x + 1))) / rooms_x;
  const float r_height = (block_size.y - (offset * (rooms_y + 1))) / rooms_y;

  glm::vec2 temp_pos = center - extent;
  temp_pos += offset;
  // finding the position of our starting index withim the block
  for (size_t i = 0; i < sx; ++i) {
    temp_pos.x += r_width + offset;
  }

  for (size_t i = 0; i < sy; ++i) {
    temp_pos.y += r_height + offset;
  }

  // initializing the starting rooms in this block
  map_room* starting_room = new map_room;
  starting_room->extent = {r_width, r_height};
  starting_room->center = temp_pos + starting_room->extent;
  // starting_room->parent_block = this;
  starting_room->plot_pos = temp_pos;

  rooms[sy][sx] = starting_room;

  int_fast8_t cx = sx;
  int_fast8_t cy = sy;

  uint_fast8_t current_steps = 1;

  while (current_steps < n) {
    uint_fast8_t r = rand() % 4;

    switch (r) {
      case 0: {
        temp_pos.x += (r_width + offset);
        ++cx;
        break;
      }
      case 1: {
        temp_pos.y += (r_height + offset);
        ++cy;
        break;
      }
      case 2: {
        temp_pos.x -= (r_width + offset);
        --cx;
        break;
      }
      default: {
        temp_pos.y -= (r_height + offset);
        --cy;
        break;
      }
    }  // switch end

    // validating our position
    // failure repositons to the opposite direction
    if (cx < 0) {
      uint_fast8_t ti = cx = 1;
      temp_pos.x = starting_room->plot_pos.x;
      while (ti < sx) {
        temp_pos.x -= (r_width + offset);
        ++ti;
      }
    }

    if (cy < 0) {
      uint_fast8_t ti = cy = 1;
      temp_pos.y = starting_room->plot_pos.y;
      while (ti < sy) {
        temp_pos.y -= (r_height + offset);
        ++ti;
      }
    }

    if (cx >= rooms_x) {
      uint_fast8_t ti = cx = rooms_x - 2;
      temp_pos.x = starting_room->plot_pos.x;
      while (ti > sx) {
        temp_pos.x += (r_width + offset);
        --ti;
      }
    }

    if (cy >= rooms_y) {
      uint_fast8_t ti = cy = rooms_y - 2;
      temp_pos.y = starting_room->plot_pos.y;
      while (ti > sy) {
        temp_pos.y += (r_height + offset);
        --ti;
      }
    }

    // inserting a new room if none in this location
    if (!rooms[cy][cx]) {
      map_room* curr = new map_room;
      curr->plot_pos = temp_pos;
      curr->extent = starting_room->extent;
      curr->center = temp_pos + curr->extent;
      // curr->parent_block = this;
      rooms[cy][cx] = curr;
      ++current_steps;
    }

  }  // while end
}

world_map::world_map()
    : length(0),
      width(0),
      height(0),
      seed(time(NULL)),
      block_size(0),
      /*bw_height(0),
      bw_width(0),*/
      offset(0),
      br_height(0),
      br_width(0),
      r_offset(0) {}

world_map::world_map(uint_fast8_t w, uint_fast8_t h, uint_fast8_t l)
    : width(w),
      height(h),
      length(l),
      seed(time(NULL)),
      block_size(0),
      /*bw_height(0),
      bw_width(0),*/
      offset(0),
      br_height(0),
      br_width(0),
      r_offset(0) {
  blocks.resize(height);
  for (auto& c : blocks) {
    c.resize(width);
  }

  // safety check to prevent infinte loop during generation
  assert(length <= width * height);
}

world_map::~world_map() {
  // cleaning up for now. We might want to save some of this data
  for (size_t y = 0; y < blocks.size(); ++y) {
    for (size_t x = 0; x < blocks[y].size(); ++x) {
      map_block* curr = blocks[y][x];
      if (curr) {
        for (size_t i = 0; i < curr->rooms.size(); i++) {
          for (size_t j = 0; j < curr->rooms[i].size(); j++) {
            map_room* croom = curr->rooms[i][j];
            delete croom;
          }
        }
      }
      delete curr;
    }
  }
}

void world_map::set_block_size(uint_fast16_t w, uint_fast16_t h,
                               uint_fast8_t o) {
  /*bw_width = w;
  bw_height = h;*/
  block_size.x = w;
  block_size.y = h;
  offset = o;
}

void world_map::set_block_space(uint_fast8_t w, uint_fast8_t h,
                                uint_fast8_t room_offset) {
  br_width = w;
  br_height = h;
  r_offset = room_offset;
}

void world_map::generate_map_blockout(glm::vec2 midpoint) {
  // center position of our map
  const uint_fast8_t sx = floor(width / 2 + 0.5f);
  const uint_fast8_t sy = floor(height / 2 + 0.5f);

  // creating our starting block based on center pos
  map_block* starting_block = new map_block(sx, sy);
  starting_block->center = midpoint;
  starting_block->extent = block_size;

  int_fast8_t cx = sx;
  int_fast8_t cy = sy;

  uint_fast8_t current_steps = 0;

  glm::vec2 current_pos = midpoint;

  // seeding random
  srand(seed);

  // generating blockout
  while (current_steps < length) {
    uint_fast8_t r = floor(rand() % 4);
    // determining the direction we are moving to in this frame based on random
    switch (r) {
      case 0: {
        ++cx;
        current_pos.x += block_size.x + offset;
        break;
      }
      case 1: {
        ++cy;
        current_pos.y += block_size.y + offset;
        break;
      }
      case 2: {
        --cx;
        current_pos.x -= (block_size.x + offset);
        break;
      }
      default: {
        --cy;
        current_pos.y -= (block_size.y + offset);
        break;
      }
    }  // switch r

    // validating our position
    // failure repositons to the opposite direction
    if (cx < 0) {
      uint_fast8_t ti = cx = 1;
      current_pos.x = midpoint.x;
      while (ti < sx) {
        current_pos.x -= (block_size.x + offset);
        ++ti;
      }
    }

    if (cy < 0) {
      uint_fast8_t ti = cy = 1;
      current_pos.y = midpoint.y;
      while (ti < sy) {
        current_pos.y -= (block_size.y + offset);
        ++ti;
      }
    }

    if (cx >= width) {
      uint_fast8_t ti = cx = width - 2;
      current_pos.x = midpoint.x;
      while (ti > sx) {
        current_pos.x += (block_size.x + offset);
        --ti;
      }
    }

    if (cy >= height) {
      uint_fast8_t ti = cy = height - 2;
      current_pos.y = midpoint.y;
      while (ti > sy) {
        current_pos.y += (block_size.y + offset);
        --ti;
      }
    }

    // checking if the new position is valid or occupied
    if (!blocks[cy][cx]) {
      map_block* curr = new map_block(cx, cy);
      curr->center = current_pos;
      curr->extent = block_size;
      blocks[cy][cx] = curr;

      // a block was generated, take a step
      ++current_steps;
    }

  }  // while loop end

  // connecting all blocks with their neighbors
  for (size_t i = 0; i < blocks.size(); i++) {
    for (size_t j = 0; j < blocks[i].size(); j++) {
      if (blocks[i][j]) {
        map_block* curr = blocks[i][j];
        if (i > 0) {
          if (blocks[i - 1][j]) {
            curr->neighbors.push_back(blocks[i - 1][j]);
          }
        }

        if (j > 0) {
          if (blocks[i][j - 1]) {
            curr->neighbors.push_back(blocks[i][j]);
          }
        }

        if (i < height - 1) {
          if (blocks[i + 1][j]) {
            curr->neighbors.push_back(blocks[i + 1][j]);
          }
        }

        if (j < width - 1) {
          if (blocks[i][j + 1]) {
            curr->neighbors.push_back(blocks[i][j + 1]);
          }
        }
      }
    }
  }  // for i end
}

void world_map::generate_block_rooms(uint_fast8_t min_rooms,
                                     uint_fast8_t max_rooms) {
  for (size_t y = 0; y < blocks.size(); ++y) {
    for (size_t x = 0; x < blocks[y].size(); x++) {
      if (blocks[y][x]) {
        blocks[y][x]->generate_rooms(min_rooms, max_rooms, br_width, br_height,
                                     block_size, r_offset);
      }
    }
  }
}

void world_map::get_dimensions(glm::vec2& min, glm::vec2& max) {
  min = {std::numeric_limits<float>::max(), std::numeric_limits<float>::max()};
  max = {std::numeric_limits<float>::min(), std::numeric_limits<float>::min()};

  for (size_t y = 0; y < blocks.size(); y++) {
    for (size_t x = 0; x < blocks[y].size(); x++) {
      if (blocks[y][x]) {
        map_block* curr = blocks[y][x];
        if (curr->center.x < min.x) {
          min.x = curr->center.x - curr->extent.x;
        }

        if (curr->center.x > max.x) {
          max.x = curr->center.x + curr->extent.x;
        }

        if (curr->center.y < min.y) {
          min.y = curr->center.y - curr->extent.y;
        }

        if (curr->center.y > max.y) {
          max.y = curr->center.y + curr->extent.y;
        }
      }
    }
  }  // for y end
}