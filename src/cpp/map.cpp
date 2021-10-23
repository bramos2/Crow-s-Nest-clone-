
#include "../hpp/map.hpp"
#include "../hpp/game_manager.hpp"

namespace crow {
    void room::generate_tilemap() {
        tiles = crow::tile_map(width, length);
        tiles.create_map();
        pather = crow::theta_star(&tiles);

        for (crow::interactible*& i : objects) {
            if (i && i->type != crow::object_type::DOOR) {
                tiles.map[i->y][i->x]->is_open = false;
            }
        }

    }

float2e room::get_tile_wpos(unsigned int const x, unsigned int const y) {
  return tiles.get_tile_wpos(x, y);
}

float2e room::get_tile_wpos(tile* const tile) {
  return tiles.get_tile_wpos(tile);
}

tile* room::get_tile_at(float2e const pos)  {
  return tiles.get_tile_at(pos);
}

std::vector<float2e> room::get_path(float2e start, float2e goal) {
  std::vector<float2e> result;
  // converting given positions into tiles
  tile* s = get_tile_at(start);
  tile* g = get_tile_at(goal);
  // prevents player from pathing to tiles that don't exist
  if (!g || !g->is_open) return result;
  // crow::theta_star path_finder;
  // pather.set_weight(1.2f);
  // initializing the path finder, returns false if s or g are null
  if (pather.set_theta_star(s, g)) {
    pather.search_theta_star();
    std::vector<tile*> path = pather.get_path();
    // converting path from the search into world space coordinates
    if (!path.empty()) {
      for (size_t i = 0; i < path.size(); ++i) {
        result.push_back(get_tile_wpos(path[i]));
      }
    }
  }
  return result;
}


}  // namespace crow