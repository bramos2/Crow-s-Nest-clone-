#pragma once
#include <cstdint>
#include <vector>

struct tile {
  std::uint_fast16_t col, row;
  std::vector<tile*> neighbors;
  bool is_open = true;
  bool in_path = false;

  tile() : col(0), row(0), is_open(true), in_path(false) {}
  tile(std::uint_fast16_t _row, std::uint_fast16_t _col)
      : row(_row), col(_col), is_open(true), in_path(false) {}
  ~tile() = default;

  // manhatan distance calculation
  float m_distance(tile* const to) const;

  // uses the Parametric algorythim to check LOS between *this and *to
  bool line_of_sight(tile* const to,
                     std::vector<std::vector<tile*>> const map) const;
};

// linked grid
class tile_map {
  std::uint_fast16_t width, height;

 public:
  std::vector<std::vector<tile*>> map;
  tile_map() = default;
  tile_map(std::uint_fast16_t _width, std::uint_fast16_t _height);

  ~tile_map();
  
  void create_map();
    
  std::uint_fast16_t get_width() { return width; }
  std::uint_fast16_t get_height() { return height; }
};


