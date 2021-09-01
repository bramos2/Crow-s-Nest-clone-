#include <cmath>

#include "../hpp/tile.hpp"

using fast_int = std::int_fast32_t;

// tile functions
float tile::m_distance(tile* const to) const {
  return static_cast<float>((this->col - to->col) + (this->row - to->row));
}

bool tile::line_of_sight(tile* const to,
                         std::vector<std::vector<tile*>> const map) const {
  fast_int x0 = this->col;
  fast_int x1 = to->col;

  fast_int y0 = this->row;
  fast_int y1 = to->row;

  fast_int deltax = std::fabs(x1 - x0);
  fast_int deltay = std::fabs(y1 - y0);

  fast_int total_tiles = (deltax > deltay) ? deltax : deltay;
  for (size_t i = 0; i < total_tiles; ++i) {
    float r = static_cast<float>(i) / total_tiles;
    fast_int x = (x1 - x0) * r + x0 + 0.5f;
    fast_int y = (y1 - y0) * r + y0 + 0.5f;

    if (!map[x][y]->is_open) {
      return false;
    }
  }

  return true;
}

// tile_map functions
tile_map::tile_map(std::uint_fast16_t w, std::uint_fast16_t h) : width(w), height(h) {
  map.resize(height);
  for (auto& column : map) {
    column.resize(width);
  }
}

tile_map::~tile_map() {
  if (map.size() > 0) {
    for (size_t i = 0; i < map.size(); ++i) {
      for (size_t j = 0; j < map[i].size(); ++j) {
        tile* curr = map[i][j];
        delete curr;
      }
    }
  }
}

void tile_map::create_map() {
  for (size_t row = 0; row < height; row++) {
    for (size_t col = 0; col < width; col++) {
      tile* current = new tile(col, row);
      map[col][row] = current;
    }
  }

  for (size_t row = 0; row < height; row++) {
    for (size_t col = 0; col < width; col++) {
      // -c +r    c +r     +c +r
      // -c r     [c r]      +c r
      // -c -r    c -r      c -r

      for (size_t r = (row != 0) ? row - 1 : 0,
                  rmax = (row != height - 1) ? row + 1 : row;
           r <= rmax; ++r) {
        for (size_t c = (col != 0) ? col - 1 : 0,
                    cmax = (col != width - 1) ? col + 1 : col;
             c <= cmax; ++c) {
          if (c == col && r == row) continue;
          map[col][row]->neighbors.push_back(map[c][r]);
        }
      }
    }
  }
}
