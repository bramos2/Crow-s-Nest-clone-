#pragma once
#include <cstdint>
#include <vector>

struct tile {
  uint_fast16_t col, row;
  std::vector<tile*> neighbors;

  tile() { neighbors.reserve(4); }
  tile(uint_fast16_t _col, uint_fast16_t _row) : col(_col), row(_row) {
    // neighbors.resize(4);
  }
  ~tile() = default;

 private:
};

// linked grid
class tile_map {
 public:
  uint_fast16_t width, height;
  std::vector<std::vector<tile*>> map;
  tile_map() = default;
  tile_map(uint_fast16_t _width, uint_fast16_t _height)
      : width(_width), height(_height) {
    map.resize(width);
    for (auto& column : map) {
      column.resize(height);
    }
  }
  ~tile_map() = default;

  auto create_map() -> int;

 private:
};

auto tile_map::create_map() -> int {
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

      /*map[col][row]->neighbors.push_back(map[col - 1][row + 1]);
      map[col][row]->neighbors.push_back(map[col][row + 1]);
      map[col][row]->neighbors.push_back(map[col + 1][row + 1]);

      map[col][row]->neighbors.push_back(map[col - 1][row]);
      map[col][row]->neighbors.push_back(map[col][row]);
      map[col][row]->neighbors.push_back(map[col + 1][row]);

      map[col][row]->neighbors.push_back(map[col - 1][row - 1]);
      map[col][row]->neighbors.push_back(map[col][row - 1]);
      map[col][row]->neighbors.push_back(map[col + 1][row - 1]);*/
    }
  }
}
