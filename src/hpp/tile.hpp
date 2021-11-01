#pragma once

#include <cstdint>
#include <vector>
#include "math_types.hpp"

namespace crow {
    struct tile {
        bool is_open = true;
        std::uint_fast16_t col, row;
        std::vector<tile*> neighbors;
       // bool in_path = false;
    
        tile() : col(0), row(0), is_open(true)/*, in_path(false)*/{}
        tile(std::uint_fast16_t _row, std::uint_fast16_t _col)
            : row(_row), col(_col), is_open(true)/*, in_path(false)*/ {}
        ~tile() = default;
    
        // manhatan distance calculation
        float m_distance(tile* const to) const;
        // TODO: FIX BUG CAUSING CRASH in line_of_sight
        // uses the Parametric algorythim to check LOS between *this and *to
        bool line_of_sight(tile* const to,
                           std::vector<std::vector<tile*>> const map) const;
    };

    // linked grid
    class tile_map {
        std::uint_fast16_t width, height;

     public:
        // the widht and height of a tile in world space
        float tile_w, tile_h;
        float2e min_room_pos;
        float2e max_room_pos;
        std::vector<std::vector<tile*>> map;
        tile_map() = default;
        tile_map(std::uint_fast16_t _width, std::uint_fast16_t _height);
        
        ~tile_map();
    
        void clean_map();
        void create_map();
        //void debug_print_map(std::vector<tile*>& path);
        std::uint_fast16_t get_width() { return width; }
        std::uint_fast16_t get_height() { return height; }
    
        float2e get_tile_wpos(std::int_fast32_t const x, std::int_fast32_t const y);
        float2e get_tile_wpos(tile* const tile);
        tile* get_tile_at(float2e const pos);
    };

}  // namespace crow
