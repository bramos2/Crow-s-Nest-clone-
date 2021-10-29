
#include "../hpp/map.hpp"
#include "../hpp/game_manager.hpp"

namespace crow {
    void room::generate_debug_collision_display() {
        for (int i = 0; i < tiles.get_height(); i++) {
            for (int j = 0; j < tiles.get_width(); j++) {
                if (get_tile_at(get_tile_wpos(j, i))->is_open) continue;
                //float4x4_a furns1 = IdentityM_a(); furns1[3][0] = 10 - rooms[0][0].width * 0.5f + 0.5f; furns1[3][2] = 14 - rooms[0][0].length * 0.5f + 0.5f;
                float2e pos = get_tile_wpos(j, i);
                float4x4_a furns1 = IdentityM_a(); furns1[3][0] = pos.x; furns1[3][2] = pos.y;
                furniture_matrices.push_back((DirectX::XMMATRIX&)furns1);
                furniture_meshes.push_back(game_manager::mesh_types::CUBE);
                furniture_textures.push_back(-1);
                //rooms[0][0].furniture_matrices.push_back((DirectX::XMMATRIX&)furns1);
                //rooms[0][0].furniture_meshes.push_back(game_manager::mesh_types::CUBE);
                //rooms[0][0].furniture_textures.push_back(-1);
            }
        }
    }

    void room::load_entities(game_manager& state) {
        // temporary. this should not be called in the final version at all
        generate_debug_collision_display();

        // first we need to know what our entity and total indices will be
        size_t initial_index = state.entities.current_size;
        const unsigned int total_entities = objects.size() + furniture_matrices.size();

        // then we need to allocate state.entities
        state.entities.allocate_and_init(total_entities);

        // floor
        state.entities.allocate_and_init(1);
        object_indices.push_back(initial_index++);
        state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[game_manager::mesh_types::CUBE];
        state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::FLOOR1];
        float4x4_a floor_size = (float4x4_a&)state.entities.world_matrix[object_indices.back()];
        floor_size[0][0] = width;
        floor_size[1][1] = 0.1f;
        floor_size[2][2] = length;
        state.entities.world_matrix[object_indices.back()] = (DirectX::XMMATRIX&)floor_size;
        
        // walls
        state.entities.allocate_and_init(4);
        for (int i = 0; i < 4; i++) {
            object_indices.push_back(initial_index++);
            state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[game_manager::mesh_types::CUBE];
            state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::FLOOR1];
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

        // loading in the meshes for the room
        while (furniture_matrices.size()) {
            object_indices.push_back(initial_index++);
            state.entities.world_matrix[object_indices.back()] = furniture_matrices.back();
            state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[furniture_meshes.back()];
            if (furniture_meshes.back() == -1) state.entities.s_resource_view[object_indices.back()] = nullptr;
            else state.entities.s_resource_view[object_indices.back()] = state.textures[furniture_meshes.back()];

            furniture_matrices.pop_back();
            furniture_textures.pop_back();
            furniture_meshes.pop_back();
        }

        // loading objects
        for (crow::interactible*& i : objects) {
            i->entity_index = initial_index;

            // add each level object to the entities
            object_indices.push_back(initial_index++);
            float2e pos = get_tile_wpos(i->x, i->y);
            state.entities.set_world_position(object_indices.back(), pos.x, 0.f, pos.y);
            
            switch (i->type)
            {
            case object_type::DOOR: 
            {
                crow::door* _i = (crow::door*)(i);
                if (i->x == 0 || i->x == this->width - 1) {
                    state.entities.rotate_world_matrix(object_indices.back(), 0.f, 90.f);
                }
                state.entities.scale_world_matrix(object_indices.back(), 0.35f);

                state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[game_manager::mesh_types::DOOR];

                // spawns this entity with a red hatch if it's locked
                if ((!i->is_active || (_i->panel && _i->panel->panel_status == 0) || (_i->neighbor && _i->neighbor->panel && _i->neighbor->panel->panel_status == 0))) {
                    state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::DOOR_CLOSED];
                    // additionally, tick the proper is_active bool
                    i->is_active = false;
                // use a green hatch if it's unlocked
                } else {
                    state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::DOOR_OPEN];
                    // additionally, tick the proper is_active bool
                    i->is_active = false;
                }
                break;
            }
            default:
            {
                state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[2];
                break;
            }
            }
        }
    }
    
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
