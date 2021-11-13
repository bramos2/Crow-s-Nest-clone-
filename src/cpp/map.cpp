
#include "../hpp/map.hpp"
#include "../hpp/game_manager.hpp"

namespace crow {
	void room::generate_debug_collision_display() {
		for (unsigned int i = 0; i < tiles.get_height(); i++) {
			for (unsigned int j = 0; j < tiles.get_width(); j++) {
				if (get_tile_at(get_tile_wpos(j, i))->is_open) {
					continue;
				}
				bool skip = false;
				for (auto& o : objects) {
					if (o->x == j && o->y == i) {
						skip = true;
					}
				}

				if (skip) {
					continue;
				}

				float2e pos = get_tile_wpos(j, i);
				float4x4_a furns1 = IdentityM_a(); furns1[3][0] = pos.x; furns1[3][2] = pos.y;
				furniture_matrices.push_back((DirectX::XMMATRIX&)furns1);
				furniture_meshes.push_back(game_manager::mesh_types::CUBE);
				furniture_textures.push_back(-1);
			}
		}
	}

	void room::load_entities(game_manager& state) {
		//generate_debug_collision_display();

		// first we need to know what our entity and total indices will be
		size_t initial_index = state.entities.current_size;
		const unsigned int total_entities = static_cast<unsigned int>(objects.size()) + static_cast<unsigned int>(furniture_matrices.size());

		// then we need to allocate state.entities
		state.entities.allocate_and_init(total_entities);

		// floor
		object_indices.push_back(entity::FLOOR);
		/*state.entities.allocate_and_init(1);
		object_indices.push_back(initial_index++);
		state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[game_manager::mesh_types::CUBE];
		state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::FLOOR1];
		float4x4_a floor_size = (float4x4_a&)state.entities.world_matrix[object_indices.back()];
		floor_size[0][0] = width;
		floor_size[1][1] = 0.1f;
		floor_size[2][2] = length;
		state.entities.world_matrix[object_indices.back()] = (DirectX::XMMATRIX&)floor_size;
		*/


		// walls
		object_indices.push_back(entity::WALL_U);
		object_indices.push_back(entity::WALL_D);
		object_indices.push_back(entity::WALL_L);
		object_indices.push_back(entity::WALL_R);
		// state.entities.allocate_and_init(4);
		 /*for (int i = 0; i < 4; i++) {
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
		 }*/

		 // loading in the meshes for the room
		while (furniture_matrices.size()) {
			object_indices.push_back(initial_index++);
			state.entities.world_matrix[object_indices.back()] = furniture_matrices.back();
			state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[furniture_meshes.back()];
			if (furniture_textures.back() == -1) state.entities.s_resource_view[object_indices.back()] = nullptr;
			else state.entities.s_resource_view[object_indices.back()] = state.textures[furniture_textures.back()];

			// bruteforcing to ensure barrels and crates use their emmissive texture
			//if (furniture_textures.back() == game_manager::texture_list::CRATE_BARREL) {
			//	state.entities.emissive[object_indices.back()] = state.textures[game_manager::texture_list::CRATE_BARREL_E];
			//}

			furniture_matrices.pop_back();
			furniture_textures.pop_back();
			furniture_meshes.pop_back();
		}

		// loading objects
		for (crow::interactible*& i : objects) {
			i->entity_index = static_cast<int>(initial_index);

			// add each level object to the entities
			object_indices.push_back(initial_index++);
			float2e pos = get_tile_wpos(i->x, i->y);
			state.entities.set_world_position(object_indices.back(), pos.x, 0.f, pos.y);

			switch (i->type)
			{
			case object_type::DOOR_PANEL: {
				state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[game_manager::mesh_types::CONSOLE3];
				state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::CONSOLE3];

				// automatically rotate this thing based on its position
				if (i->x == this->width - 1) {
					state.entities.rotate_world_matrix(object_indices.back(), -90.f, -90.f);
				} else if (i->x == 0) { // left wall
					state.entities.rotate_world_matrix(object_indices.back(), 0.f, 90.f, 90.f);
				} else if (i->y == this->length - 1) { // top wall
					state.entities.rotate_world_matrix(object_indices.back(), 0, 180.f, 90.f);
				} else { // bottom wall
					state.entities.rotate_world_matrix(object_indices.back(), 0, 0, 90.f);
				}

				// move this thing up
				XMFLOAT3 door_pos = state.entities.get_world_position(object_indices.back());
				state.entities.set_world_position(object_indices.back(), door_pos.x, door_pos.y + 2, door_pos.z);

				state.entities.scale_world_matrix(object_indices.back(), 0.8f);
				} break;
			case object_type::OXYGEN_CONSOLE:
				state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[game_manager::mesh_types::CONSOLE2];
				state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::CONSOLE2];
				// automatically rotate this thing based on its position
				if (i->x == this->width - 1) {
					state.entities.rotate_world_matrix(object_indices.back(), 0.f, -90.f);
				} else if (i->x == 0) {
					state.entities.rotate_world_matrix(object_indices.back(), 0.f, 90.f);
				} else if (i->y == this->length - 1) {
					// face it down
					state.entities.rotate_world_matrix(object_indices.back(), 0.f, 180.f);
				} else {
					// default orientation
				}

				state.entities.scale_world_matrix(object_indices.back(), 0.8f);
			case object_type::PRESSURE_CONSOLE: {
				// gonna use julio's version of this unless we have any problems
				/*
				state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[game_manager::mesh_types::CONSOLE1];
				state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::CONSOLE1_D];
				state.entities.specular[object_indices.back()] = state.textures[game_manager::texture_list::CONSOLE1_S];
				// automatically rotate this thing based on its position
				if (i->x == this->width - 1) {
					state.entities.rotate_world_matrix(object_indices.back(), 0.f, -90.f);
				} else if (i->x == 0) {
					state.entities.rotate_world_matrix(object_indices.back(), 0.f, 90.f);
				} else if (i->y == this->length - 1) {
					// default orientation
				} else {
					// face it down
					state.entities.rotate_world_matrix(object_indices.back(), 0.f, 180.f);
				}

				state.entities.scale_world_matrix(object_indices.back(), 0.075f);
				*/
				
				state.entities.scale_world_matrix(object_indices.back(), 0.04f);
				state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[game_manager::mesh_types::CONSOLE1];
				state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::CONSOLE1_D];
				state.entities.specular[object_indices.back()] = state.textures[game_manager::texture_list::CONSOLE1_S];
				break;
			}
			case object_type::DOOR:
			{
				//crow::door* _i = (crow::door*)(i);
				if (i->x == 0 || i->x == this->width - 1) {
					state.entities.rotate_world_matrix(object_indices.back(), 0.f, 90.f);
				}
				state.entities.scale_world_matrix(object_indices.back(), 0.35f);

				state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[game_manager::mesh_types::DOOR];

				// spawns this entity with a red hatch if it's locked, the door should be made locked upon creation if you want it to be locked. 
				// NO NEED FOR OVER COMPLICATION. 
				if ((!i->is_active)) {
					state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::DOOR_CLOSED];
					// additionally, tick the proper is_active bool the door is already locked, or should otherwise, it would be impossible to reach this line
					//i->is_active = false;
				// use a green hatch if it's unlocked
				}
				else {
					state.entities.s_resource_view[object_indices.back()] = state.textures[game_manager::texture_list::DOOR_OPEN];
					// additionally, tick the proper is_active bool
				    //i->is_active = true;
				}
				break;
			}
			default:
			{
				state.entities.mesh_ptrs[object_indices.back()] = &state.all_meshes[game_manager::mesh_types::CUBE];
				break;
			}
			}
		}
	}

	void room::generate_tilemap() {
		tiles = crow::tile_map(width, length);
		tiles.create_map();

		for (crow::interactible*& i : objects) {
			if (i && i->type != crow::object_type::DOOR) {
				tiles.map[i->y][i->x]->is_open = false;
			}
		}

		// tiles need to be in their final state before calling this constructor
		//pather = crow::theta_star(&tiles);
	}

	void room::initialize_pather()
	{
		pather = crow::theta_star(&tiles);
	}

	void room::update_room_doors(std::vector<ID3D11ShaderResourceView*>& textures, entities& entities)
	{
		for (auto& o : objects) {
			if (o->type == crow::object_type::DOOR || o->type == crow::object_type::EXIT) {
				if (o->is_active) {
					entities.s_resource_view[o->entity_index] = textures[game_manager::texture_list::DOOR_OPEN];
				}
				else {
					entities.s_resource_view[o->entity_index] = textures[game_manager::texture_list::DOOR_CLOSED];
				}
			}
		}
	}

	float2e room::get_tile_wpos(unsigned int const x, unsigned int const y) {
		return tiles.get_tile_wpos(x, y);
	}

	float2e room::get_tile_wpos(tile* const tile) {
		return tiles.get_tile_wpos(tile);
	}

	tile* room::get_tile_at(float2e const pos) {
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
			//tiles.debug_print_map(path);
			// converting path from the search into world space coordinates
			if (!path.empty()) {
				for (size_t i = 0; i < path.size(); ++i) {
					result.push_back(get_tile_wpos(path[i]));
				}
			}
		}
		return result;
	}

	int room::has_broken_console() {
		int result = 0;
		for (crow::interactible*& i : objects) {
			switch (i->type) {
			// ONLY check these types of objects
			case object_type::OXYGEN_CONSOLE:
			case object_type::PRESSURE_CONSOLE:
			case object_type::POWER_CONSOLE:
				if (i->is_broken) result = 1;
				else if (result == 0) result = -1;
				// broken console has priority over working console
			}
		}
		return result;
	}

}  // namespace crow
