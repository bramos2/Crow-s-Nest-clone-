#pragma once
#include <liblava/lava.hpp>

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <memory>
#include <optional>
#include <vector>

#include "collision.hpp"

namespace crow {

#define WORLD_SEED std::time(NULL)
constexpr int block_world_width = 80;
constexpr int block_world_height = 80;
constexpr int block_world_padding = 5;
constexpr int room_world_padding = 5;

struct map_room {
  int const world_x, world_y, width, height;
  std::vector<std::shared_ptr<crow::map_room>> neighbors;
  crow::collision_universe collision_universe;
};

template <int br_width, int br_height>
struct map_block {
  // TODO: Make x and y const.
  int block_index_x, block_index_y;
  std::vector<std::shared_ptr<crow::map_block<br_width, br_height>>> neighbors;
  std::vector<crow::map_room> room_list;
  void generate_rooms(int min_rooms, int max_rooms);
};

template <int br_width, int br_height>
struct world_map {
  std::array<std::array<std::shared_ptr<crow::map_block<br_width, br_height>>,
                        br_width>,
             br_height>
      block_grid;
  void generate_blocks(int blocks_count);
  void generate_rooms(int min_rooms, int max_rooms);
  void generate_adjacencies();
};

template <int br_width, int br_height>
void world_map<br_width, br_height>::generate_blocks(int blocks_count) {
  // center position of our map
  int const starting_index_x = br_width / 2;
  int const starting_index_y = br_height / 2;
  crow::map_block<br_width, br_height> starting_block;
  int current_index_x = starting_index_x;
  int current_index_y = starting_index_y;
  std::srand(WORLD_SEED);

  // generating blockout
  int current_blocks_count = 0;
  while (current_blocks_count < blocks_count) {
    unsigned random_number = std::floor(rand() % 4);
    // determining the direction we are moving to in this frame based on random
    switch (random_number) {
      case 0: {
        ++current_index_x;
        break;
      }
      case 1: {
        ++current_index_y;
        break;
      }
      case 2: {
        --current_index_x;
        break;
      }
      default: {
        --current_index_y;
        break;
      }
    }

    // Reset position if the current position is out of bounds.
    [[unlikely]] if (current_index_x < 0 || current_index_x > br_width ||
                     current_index_y < 0 || current_index_y > br_height) {
      current_index_x = starting_index_x;
      current_index_y = starting_index_y;
    }
    // Check if the current position is occupied by another block.
    if (block_grid[current_index_y][current_index_x] != nullptr) {
      continue;
    }

    // Create a new block here.
    // crow::map_block<br_width, br_height> curr;
    auto curr = std::make_shared<crow::map_block<br_width, br_height>>();
    curr->block_index_x = current_index_x;
    curr->block_index_y = current_index_y;
    block_grid[current_index_y][current_index_x] = curr;
    current_blocks_count++;
  }

  // connecting all blocks with their neighbors
  for (int i = 0; i < br_width; i++) {
    for (int j = 0; j < br_height; j++) {
      if (block_grid[i][j] != nullptr) {
        auto current_block = block_grid[i][j];
        if (i > 0) {
          if (block_grid[i - 1][j] != nullptr) {
            auto unwrapped_block = block_grid[i - 1][j];
            current_block->neighbors.push_back(unwrapped_block);
          }
        }
        if (j > 0) {
          if (block_grid[i][j - 1] != nullptr) {
            auto unwrapped_block = block_grid[i][j - 1];
            current_block->neighbors.push_back(unwrapped_block);
          }
        }
        if (i < br_width - 1) {
          if (block_grid[i + 1][j] != nullptr) {
            auto unwrapped_block = block_grid[i + 1][j];
            current_block->neighbors.push_back(unwrapped_block);
          }
        }
        if (j < br_height - 1) {
          if (block_grid[i][j + 1] != nullptr) {
            auto unwrapped_block = block_grid[i][j + 1];
            current_block->neighbors.push_back(unwrapped_block);
          }
        }
      }
    }
  }
}

template <int br_width, int br_height>
void map_block<br_width, br_height>::generate_rooms(int min_rooms,
                                                    int max_rooms) {
  auto overlaps_room = [&](std::vector<crow::map_room>& rooms,
                           int current_world_x, int current_world_y,
                           int current_room_width,
                           int current_room_height) -> bool {
    for (const crow::map_room& room : rooms) {
      if (
          // If the other room is rightward of the current room's left boundary:
          room.world_x + room.width > current_world_x &&
          // If the other room is leftward of
          // the current room's right boundary:
          room.world_x < current_world_x + current_room_width &&
          // If the other room is below of the current room's top
          // boundary:
          room.world_y + room.height > current_world_y &&
          // If the other room is above of the
          // current room's bottom boundary:
          room.world_y < current_world_y + current_room_height) {
        return true;
      }
    }
    return false;
  };

  // Generate either the minimum number of possible rooms or some random index
  // that is higher.
  int random_rooms_count = std::max<int>(rand() % max_rooms, min_rooms);
  for (int i = 0; i < random_rooms_count; i++) {
    // TODO: These should be read from a room prefab pool.
    int room_width = rand() % 10 + 10;
    int room_height = rand() % 10 + 10;
    int x = crow::block_world_width / 2;
    int y = crow::block_world_height / 2;
    // Random walk this room.
    while (overlaps_room(this->room_list,
                         x + this->block_index_x * crow::block_world_width -
                             crow::room_world_padding,
                         y + this->block_index_y * crow::block_world_height -
                             crow::room_world_padding,
                         room_width + crow::room_world_padding * 2,
                         room_height + crow::room_world_padding * 2)) {
      // Increment x and y in by (-1 | 0 | 1)
      x += (rand() % 3) - 1;
      y += (rand() % 3) - 1;
      // Keep this room within bounds.
      [[unlikely]] if (x + room_width > crow::block_world_width || x < 0 ||
                       y + room_height > crow::block_world_height || y < 0) {
        x = crow::block_world_width / 2;
        y = crow::block_world_height / 2;
      }
    }
    this->room_list.push_back(crow::map_room{
        .world_x = x + this->block_index_x * crow::block_world_width,
        .world_y = y + this->block_index_y * crow::block_world_height,
        .width = room_width,
        .height = room_width,
    });
  }
}

template <int br_width, int br_height>
void world_map<br_width, br_height>::generate_rooms(int min_rooms,
                                                    int max_rooms) {
  for (auto& block_x : block_grid) {
    for (auto& block : block_x) {
      if (block != nullptr) {
        auto unwrapped_block = block;
        unwrapped_block->generate_rooms(min_rooms, max_rooms);
      }
    }
  }
}

template <int br_width, int br_height>
void world_map<br_width, br_height>::generate_adjacencies() {
  // TODO: Generate flat rooms ahead of time.
  std::vector<std::shared_ptr<crow::map_room>> flat_rooms;
  for (auto& block_y : this->block_grid) {
    for (auto& block_x : block_y) {
      if (block_x != nullptr) {
        for (crow::map_room& room : block_x->room_list) {
          auto shared_room = std::make_shared<crow::map_room>(room);
          flat_rooms.push_back(shared_room);
        }
      }
    }
  }
  for (auto& current_room : flat_rooms) {
    for (auto& other_room : flat_rooms) {
      if (&current_room == &other_room) {
        continue;
      }
      int x = (other_room->world_x + other_room->width / 2) -
              (current_room->world_x + current_room->width / 2);
      int y = (other_room->world_y + other_room->height / 2) -
              (current_room->world_y + current_room->height / 2);
      if (x * x + y * y < 40 * 40) {
        const std::shared_ptr<map_room>& pneighbor = other_room;
        current_room->neighbors.push_back(pneighbor);
      }
    }
  }
}

}  // namespace crow
