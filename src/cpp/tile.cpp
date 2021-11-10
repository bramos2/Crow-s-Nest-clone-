#include "../hpp/tile.hpp"
#include <string>
#include <cmath>

using fast_int = std::int_fast32_t;

namespace crow {
// tile functions
float tile::m_distance(tile* const to) const {
    return std::fabsf(static_cast<float>(this->col - to->col)) + fabsf(static_cast<float>(this->row - to->row));
}

bool tile::line_of_sight(tile* const to,
                         std::vector<std::vector<tile*>> const map) const {
  int x0 = this->col;
  int x1 = to->col;

  int y0 = this->row;
  int y1 = to->row;

  int deltax = static_cast<int>(std::fabs(x1 - x0));
  int deltay = static_cast<int>(std::fabs(y1 - y0));

  int total_tiles = (deltax >= deltay) ? deltax : deltay;
  for (size_t i = 0; i < total_tiles; ++i) {
    float r = static_cast<float>(i) / total_tiles;
    int x = (x1 - x0) * r + x0 + 0.5f;
    int y = (y1 - y0) * r + y0 + 0.5f;

    if (!map[y][x]->is_open) {
      return false;
    }
  }

  return true;
}

// tile_map functions
tile_map::tile_map(std::uint_fast16_t w, std::uint_fast16_t h)
    : width(w),
      height(h),
      tile_w(1.0f),
      tile_h(1.0f),
      max_room_pos(float2e(-w / 2.0f, -h / 2.0f)),
      min_room_pos(float2e(w / 2.0f, h / 2.0f)) {
  map.resize(height);
  for (auto& column : map) {
    column.resize(width);
  }
  create_map();
}

tile_map::~tile_map() { clean_map(); }

void tile_map::clean_map() {
  for (auto& row : map) {
    for (crow::tile* t : row) {
      if (t) {
        delete t;
      }
    }
  }
}

void tile_map::create_map() {
  if (width > 0 && height > 0) {
    for (size_t y = 0; y < height; ++y) {
      for (size_t x = 0; x < width; ++x) {
        tile* current = new tile(y, x);
        map[y][x] = current;
      }
    }
    for (size_t row = 0; row < height; ++row) {
      for (size_t col = 0; col < width; ++col) {
        // -c +r    c +r     +c +r
        // -c r     [c r]      +c r
        // -c -r    c -r      c -r

        for (size_t r = (row != 0) ? row - 1 : 0,
                    rmax = (row != height - 1) ? row + 1 : row;
             r <= rmax; ++r) {
          for (size_t c = (col != 0) ? col - 1 : 0,
                      cmax = (col != width - 1) ? col + 1 : col;
               c <= cmax; ++c) {
            if (c == col && r == row) {
              continue;
            }
            map[row][col]->neighbors.push_back(map[r][c]);
          }
        }
      }
    }
  }
}

//void tile_map::debug_print_map(std::vector<tile*>& path) {
//    for (auto& t: path)
//    {
//        t->in_path = true;
//    }
//
//    std::printf("\n");
//    for (size_t y = 0; y < height; y++)
//    {
//        for (size_t x = 0; x < width; x++)
//        {
//            tile* curr = map[y][x];
//            if (curr->in_path && curr->is_open) { // in path and open
//                std::printf("x");
//            }
//            else if (curr->in_path && !curr->is_open) { // in path and closed (bad)
//                std::printf("b");
//            }
//            else if (!curr->in_path && curr->is_open) { // open
//                std::printf("0");
//            }
//            else { // closed
//                std::printf("#");
//            }
//        }
//        std::printf("\n");
//    }
//
//    for (auto& t : path)
//    {
//        t->in_path = false;
//    }
//}

float2e tile_map::get_tile_wpos(std::int_fast32_t const x,
                             std::int_fast32_t const y) {
  float2e result;
  result.y = std::floor(y - (((height * tile_h) - 1) / 2.0f));
  result.x = std::floor(x - (((width * tile_w) - 1) / 2.0f));

  return result;
}

float2e tile_map::get_tile_wpos(tile* const tile) {
  return get_tile_wpos(tile->col, tile->row);
}

tile* tile_map::get_tile_at(float2e const pos) {
  const float fy = (height * tile_h) / 2.0f + pos.y;
  const float fx = (width * tile_w) / 2.0f + pos.x;

  const unsigned int x = static_cast<uint16_t>(fx);
  const unsigned int y = static_cast<uint16_t>(fy);

  if (y < height && x < width && y >= 0 && x >= 0) {
    tile* temp = map[y][x];
    return temp;
  }

  return nullptr;
}

}  // namespace crow