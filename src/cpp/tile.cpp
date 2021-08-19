#pragma once
#include "../hpp/tile.hpp"


// tile functions
float tile::m_distance(tile* const to) const {
  return static_cast<float>((this->col - to->col) + (this->row - to->row));
}

bool tile::line_of_sight(tile* const to,
                         std::vector<std::vector<tile*>> const map) const {
  int_fast32_t x0 = this->col;
  int_fast32_t x1 = to->col;

  int_fast32_t y0 = this->row;
  int_fast32_t y1 = to->row;

  int_fast32_t deltax = fabs(x1 - x0);
  int_fast32_t deltay = fabs(y1 - y0);

  int_fast32_t total_tiles = (deltax > deltay) ? deltax : deltay;
  for (size_t i = 0; i < total_tiles; ++i) {
    float r = static_cast<float>(i) / total_tiles;
    int_fast32_t x = (x1 - x0) * r + x0 + 0.5f;
    int_fast32_t y = (y1 - y0) * r + y0 + 0.5f;

    if (!map[x][y]->is_open) {
      return false;
    }
  }

  return true;
}

//tile_map functions
tile_map::tile_map(uint_fast16_t w, uint_fast16_t h) : width(w), height(h) {
  map.resize(width);
  for (auto& column : map) {
    column.resize(height);
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
  for (size_t col = 0; col < width; col++) {
    for (size_t row = 0; row < height; row++) {
      tile* current = new tile(col, row);
      map[col][row] = current;
    }
  }

  for (size_t col = 0; col < width; col++) {
    for (size_t row = 0; row < height; row++) {
      // -c +r    c +r     +c +r
      // -c r     [c r]      +c r
      // -c -r    c -r      c -r

      for (size_t c = (col != 0) ? col - 1 : 0,
                  cmax = (col != width - 1) ? col + 1 : col;
           c <= cmax; ++c) {
        for (size_t r = (row != 0) ? row - 1 : 0,
                    rmax = (row != height - 1) ? row + 1 : row;
             r <= rmax; ++r) {
          if (c == col && r == row) continue;
          map[col][row]->neighbors.push_back(map[c][r]);
        }
      }
    }
  }
}