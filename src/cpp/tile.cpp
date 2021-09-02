#include "../hpp/tile.hpp"

#include <cmath>

using fast_int = std::int_fast32_t;

namespace crow {
// tile functions
float tile::m_distance(tile* const to) const {
  return static_cast<float>((this->col - to->col) + (this->row - to->row));
}

bool tile::line_of_sight(tile* const to,
                         std::vector<std::vector<tile*>> const map) const {
  int x0 = this->col;
  int x1 = to->col;

  int y0 = this->row;
  int y1 = to->row;

  int deltax = std::fabs(x1 - x0);
  int deltay = std::fabs(y1 - y0);

  int total_tiles = (deltax > deltay) ? deltax : deltay;
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
      max_room_pos(0),
      min_room_pos(0) {
  map.resize(height);
  for (auto& column : map) {
    column.resize(width);
  }
}

tile_map::~tile_map() {}

void tile_map::clean_map() {
  for (size_t i = 0; i < map.size(); ++i) {
    for (size_t j = 0; j < map[i].size(); ++j) {
      tile* curr = map[i][j];
      if (curr) {
        delete curr;
      }
    }
  }
}

void tile_map::create_map() {
  if (width > 0 && height > 0) {
    for (size_t row = 0; row < height; ++row) {
      for (size_t col = 0; col < width; ++col) {
        tile* current = new tile(row, col);
        map[row][col] = current;
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

glm::vec2 tile_map::get_tile_wpos(std::int_fast32_t const x,
                                  std::int_fast32_t const y) {
  glm::vec2 result;
  result.y = std::floor(y - ((height - 1) / 2.0f));
  result.x = std::floor(x - ((width - 1) / 2.0f));

  /* if (result.y < 0) {
     result.y += tile_h / 2.0f;
   } else {
     result.y -= tile_h / 2.0f;
   }

   if (result.x < 0) {
     result.x += tile_w / 2.0f;
   } else {
     result.x -= tile_w / 2.0f;
   }*/

  return result;
}

glm::vec2 tile_map::get_tile_wpos(tile* const tile) {
  return get_tile_wpos(tile->col, tile->row);
}

tile* tile_map::get_tile_at(glm::vec2 const pos) {
  float y = height / 2.0f + pos.y;
  float x = width / 2.0f + pos.x;
  if (y < height && x < width && y >= 0 && x >= 0) {
    return map[static_cast<int>(std::floor(y))]
              [static_cast<int>(std::floor(x))];
  }

  return nullptr;
  /*glm::vec2 temp = min_room_pos;
  std::uint_fast32_t y = height - 1;
  std::uint_fast32_t x = width - 1;

  for (size_t _y = 0; _y < height; ++_y) {
    temp.y += tile_h;
    if (temp.y > pos.y) {
      y = _y - 1;
      break;
    }
  }

  for (size_t _x = 0; _x < width; ++_x) {
    temp.x += tile_w;
    if (temp.x > pos.x) {
      x = _x - 1;
      break;
    }
  }

  return map[x][y];*/
}

}  // namespace crow