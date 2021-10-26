
#include "../hpp/map.hpp"
#include "../hpp/game_manager.hpp"

namespace crow {
    void room::load_entities(game_manager& state) {
        // first we need to know what our entity and total indices will be
        size_t initial_index = state.entities.current_size;
        const unsigned int total_entities = objects.size();

        // then we need to allocate state.entities
        state.entities.allocate_and_init(total_entities);

        // floor
        state.entities.allocate_and_init(1);
        object_indices.push_back(initial_index++);
        state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[2];
        float4x4_a floor_size = (float4x4_a&)state.entities.world_matrix[object_indices.back()];
        floor_size[0][0] = width;
        floor_size[1][1] = 0.1f;
        floor_size[2][2] = length;
        state.entities.world_matrix[object_indices.back()] = (DirectX::XMMATRIX&)floor_size;

        /*
        // walls
        state.entities.allocate_and_init(4);
        for (int i = 0; i < 4; i++) {
            object_indices.push_back(initial_index++);
            state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[2];
            float4x4_a wall_size = (float4x4_a&)state.entities.world_matrix[object_indices.back()];
            wall_size[1][1] = height;

            switch (i) {
            case 1: 
                wall_size[0][0] = width;
                wall_size[2][2] = 0.1f;
                wall_size[3][2] = -((int)(length / 2));
                break;
            case 0:
                wall_size[0][0] = width;
                wall_size[2][2] = 0.1f;
                wall_size[3][2] = length / 2;
                break;
            case 3: 
                wall_size[0][0] = 0.1f;
                wall_size[2][2] = length;
                wall_size[3][0] = -((int)(width / 2));
                break;
            case 2:
                wall_size[0][0] = 0.1f;
                wall_size[2][2] = length;
                wall_size[3][0] = width / 2;
                break;
            }


            state.entities.world_matrix[object_indices.back()] = (DirectX::XMMATRIX&)wall_size;
        }
        /**/

        // loading objects
        for (crow::interactible*& i : objects) {
            // add each level object to the entities
            object_indices.push_back(initial_index++);
            float2e pos = get_tile_wpos(i->x, i->y);
            state.entities.set_world_position(object_indices.back(), pos.x, 0.f, pos.y);
            state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[2];
        }
    }
    
    void room::generate_tilemap() {
        tiles = crow::tile_map(width, length);
        tiles.create_map();
        pather = crow::theta_star(&tiles);

        return; // todo::temporary bugfix
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
