#include "..\hpp\enemy_behaviors.hpp"

#include <time.h>

#include "../hpp/player_behavior.hpp"

namespace crow {

	// void ai_manager::create_b_tree() {
	//  //// allocating nodes for the tree
	//  // nodes.resize(19);
	//  //// 0 - 11 child nodes (12)
	//  // for (size_t i = 0; i < 12; i++) {
	//  //  nodes[i] = new behavior_tree::leaf_node();
	//  //}
	//
	//  //// 12 - 14 sequence nodes (3)
	//  // for (size_t i = 12; i < 15; i++) {
	//  //  nodes[i] = new behavior_tree::sequence_node();
	//  //}
	//
	//  //// 15 - 18 selector nodes (4)
	//  // for (size_t i = 15; i < 19; i++) {
	//  //  nodes[i] = new behavior_tree::selector_node();
	//  //}
	//
	//  //// assigning behaviors to leaf nodes
	//  // nodes[0]->r = &has_target;
	//
	//  //// connecting nodes to form tree
	//}

	// void ai_manager::clean_tree() {
	//  /* for (auto& n : nodes) {
	//     if (n) {
	//       delete n;
	//     }
	//   }
	//   nodes.clear();*/
	//}

	status roam_check(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;
		m.roam_timer += dt;
		if (m.roam_timer >= m.roam_total || m.curr_room->has_player) {
			m.is_roaming = false;
		}

		if (m.is_roaming) {
			result = crow::status::PASSED;
		}

		//m.print_status("roam_check", result);
		return result;
	}

	status roam_path(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;

		// we must get a ramdom path in this room if empty
		while (m.path.empty()) {
			const unsigned int x = std::rand() % m.curr_room->width;
			const unsigned int y = std::rand() % m.curr_room->length;
			float2e goal = m.curr_room->get_tile_wpos(x, y);
			float3e temp_ai_pos = m.entities->get_world_position(
				static_cast<size_t>(crow::entity::SPHYNX));
			float2e start = float2e(temp_ai_pos.x, temp_ai_pos.z);

			m.path = m.curr_room->get_path(start, goal);
		}
		// must always pass in this instance
		result = crow::status::PASSED;
		//m.print_status("roam_path", result);
		return result;
	}

	// target branch
	status has_target(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;
		if (m.target && !m.curr_room->has_player) {
			if (m.target->roomptr == m.curr_room) { // we don't need this as we reset target anyways
				result = crow::status::PASSED;
			}
			else {
				m.target = nullptr;
				m.interacting = false;
			}
		}
		else if (m.target && m.target->type == crow::object_type::PLAYER &&
			m.curr_room->has_player) {
			result = crow::status::PASSED;
		}
		//m.print_status("has_target", result);
		return result;
	}

	status target_player(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;
		// we need to check if the player is in this room
		if (!m.curr_room) {
			return result;
		}

		// will require that the room with the player has an interactible for the
		// player in it at all times
		// for (auto& obj : m.curr_room->objects) {
		//  if (obj->type == crow::object_type::PLAYER) {
		//    m.target = obj;
		//   // m.interacting = true;
		//    result = crow::status::PASSED;
		//    break;
		//  }
		//}

		if (m.curr_room->has_player && m.curr_level->p_inter->is_active) {
			m.target = m.curr_level->p_inter;
			result = crow::status::PASSED;
		}

		//m.print_status("target_player", result);
		return result;
	}

	status target_console(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;

		if (m.counter <= 0) {
			for (auto& obj : m.curr_room->objects) {
				// will need to check for the different consoles going forward
				if (obj->is_broken) {
					continue;
				}

				if (obj->type == crow::object_type::POWER_CONSOLE || obj->type == crow::object_type::PRESSURE_CONSOLE || obj->type == crow::object_type::OXYGEN_CONSOLE) {
					m.target = obj;
					// m.interacting = true;
					result = crow::status::PASSED;
					break;
				}
			}
		}

		//m.print_status("\ntarget_console", result);
		return result;
	}

	// TODO: CHANGE DOOR TARGETING
	status target_door(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;
		// we will store a pointer to all doors in this room
		std::vector<crow::interactible*> room_doors;

		// making a list of all the doors in this room
		for (auto& obj : m.curr_room->objects) {
			if (obj->type == crow::object_type::DOOR) {
				room_doors.push_back(obj);
			}
		}

		// fail if there is no doors
		if (room_doors.empty()) {
			return result;
		}

		// list of indices used to pick a random door
		std::vector<size_t> tinx;
		//tinx.push_back(0);

		// filtering doors
		for (size_t i = 0; i < room_doors.size(); i++) {
			bool add_door = false; // use primarely for locked doors
			if (!room_doors[i]->is_active) {
				int num = rand() % 100 + 1;
				if (num >= 50) {
					add_door = true;
				}
			}
			else {
				add_door = true;
			}

			if (!add_door) { continue; }

			if (tinx.empty()) {
				tinx.push_back(i);
			}

			if (i != tinx.back()) { // checking for a door that is not already in the list
				if (room_doors[i]->heat > room_doors[tinx.back()]->heat) {
					// if the next door has a higher heat than the doors already in the list, clear the list and add new door
					// TODO: Handle locked doors here instead



					tinx.clear();
					tinx.push_back(i);
				}
				else if (room_doors[i]->heat == room_doors[tinx.back()]->heat) {
					// otherwise if the door has the same heat, add it to the list
					tinx.push_back(i);
				}
				// we ignore any doors with lower heat values
			}
		}

		// we pick a random index inside the index list
		size_t choice = rand() % tinx.size();

		// the target is set and the test passes
		result = crow::status::PASSED;
		m.target = room_doors[tinx[choice]];

		//m.print_status("target_door", result);
		return result;
	}

	status target_floor(float dt, crow::ai_manager& m) { return status(); }

	// path branch
	status has_path(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;

		if (!m.path.empty() || (m.path.empty() && m.interacting)) {
			result = crow::status::PASSED;
		}

		//m.print_status("has_path", result);
		return result;
	}

	status is_path_currernt(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;
		size_t index = static_cast<size_t>(crow::entity::SPHYNX);
		// we need to check if this path is set to the current target, otherwise we
		// must clean it and get a new one
		// non moving target, use its tile
		float3e curr_pos = m.entities->get_world_position(index);
		if (m.path.empty() && m.interacting) {

			float2e target_pos;
			if (m.target->type == crow::object_type::PLAYER) {
				float3e p_pos = m.entities->get_world_position(
					static_cast<size_t>(crow::entity::WORKER));
				target_pos = { p_pos.x, p_pos.z };
			}
			else {
				target_pos = m.curr_room->get_tile_wpos(m.target->x, m.target->y);
				result = crow::status::PASSED;
			}

			if (crow::reached_destination({ 0.f, 0.f }, { curr_pos.x, curr_pos.z },
				target_pos, 1.0f)) {
				result = crow::status::PASSED;
			}
		}
		else {
			if (m.target->type != crow::object_type::PLAYER) {
				//float2e temp = m.curr_room->get_tile_wpos(m.target->x, m.target->y);

				/*if (temp == m.path[0]) {
					result = crow::status::PASSED;
				}*/
				result = crow::status::PASSED;
			}
			else {  // for player use entities
				float3e temp = m.entities->get_world_position(
					static_cast<size_t>(crow::entity::WORKER));
				float2e t = { temp.x, temp.z };

				if (crow::reached_destination({ 0.f, 0.f }, t, m.path[0], 0.2f)) {
					result = crow::status::PASSED;
				}

				//float2e diff = m.path[0] - t;
				//// if not too far appart the path is fine
				//if (std::fabsf(diff.x) <= 1.f && std::fabsf(diff.y) <= 1.f) {
				//	
				//}
			}
		}

		//m.print_status("is_path_current", result);
		return result;
	}

	status get_path(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;

		m.path.clear();
		float3e temp_ai_pos =
			m.entities->get_world_position(static_cast<size_t>(crow::entity::SPHYNX));

		float2e target_pos;
		if (m.target->type == crow::object_type::PLAYER) {
			float3e p_pos = m.entities->get_world_position(
				static_cast<size_t>(crow::entity::WORKER));
			target_pos = { p_pos.x, p_pos.z };
		}
		else {
			const crow::tile* ai_tile =
				m.curr_room->get_tile_at({ temp_ai_pos.x, temp_ai_pos.z });

			// finding the adjacent tile to the interactible
			float2e adjacent_tile =
				float2e{ static_cast<float>(ai_tile->col) -
								static_cast<float>(m.target->x),
							static_cast<float>(ai_tile->row) -
								static_cast<float>(m.target->y) };

			adjacent_tile = adjacent_tile.normalize();
			for (int i = 0; i < 2; i++) {
				if (adjacent_tile[i] >= 0.5f) {
					adjacent_tile[i] = 1.f;
					continue;
				}
				else if (adjacent_tile[i] <= -0.5f) {
					adjacent_tile[i] = -1.f;
				}
			}

			adjacent_tile = {
				adjacent_tile.x + static_cast<float>(m.target->x),
				adjacent_tile.y + static_cast<float>(m.target->y) };

			adjacent_tile =
				m.curr_room->get_tile_wpos(static_cast<int>(adjacent_tile.x),
					static_cast<int>(adjacent_tile.y));

			target_pos = adjacent_tile;
		}

		m.path = m.curr_room->get_path(float2e(temp_ai_pos.x, temp_ai_pos.y),
			target_pos);
		m.interacting = true;
		result = crow::status::PASSED;

		//m.print_status("get_path", result);
		return result;
	}

	// move branch (bugged, passing when target enters new room and player is there)
	status reached_target(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;

		/* size_t index = static_cast<size_t>(crow::entity::SPHYNX);
		 float3e curr_pos = m.entities->get_world_position(index);
		 float2e target_pos;
		 if (m.target->type == crow::object_type::PLAYER) {
		   float3e p_pos = m.entities->get_world_position(
			   static_cast<size_t>(crow::entity::WORKER));
		   target_pos = {p_pos.x, p_pos.z};
		 } else {
		   target_pos = m.curr_room->get_tile_wpos(m.target->x, m.target->y);
		 }*/

		 // || crow::reached_destination({0.f, 0.f}, {curr_pos.x, curr_pos.z},
		 // target_pos)
		if (m.path.empty()) {
			size_t index = crow::entity::SPHYNX;
			result = crow::status::PASSED;
			m.entities->velocities[index] = {
				0.f, 0.f, 0.f };
			if (!m.entities->mesh_ptrs[index]->animator->is_acting && !m.entities->mesh_ptrs[index]->animator->performed_action) {
				m.entities->mesh_ptrs[index]->animator->switch_animation(1);
				m.entities->mesh_ptrs[index]->animator->performed_action = true;
			}
		}

		//m.print_status("reached_target", result);
		return result;
	}

	status move(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;
		size_t index = crow::entity::SPHYNX;


		crow::set_velocity(m.path.back(), *m.entities, index, m.roam_speed);
		float3e curr_pos = m.entities->get_world_position(index);

		/*float2e target_pos = { 100000.f, 100000.f };
		if (m.target) {
			if (m.target->type == crow::object_type::PLAYER) {
				float3e p_pos = m.entities->get_world_position(
					static_cast<size_t>(crow::entity::WORKER));
				target_pos = { p_pos.x, p_pos.z };
			}
			else {
				target_pos = m.curr_room->get_tile_wpos(m.target->x, m.target->y);
			}
		}*/

		float2e curr_vel = float2e(dt * m.entities->velocities[index].x,
			dt * m.entities->velocities[index].z);
		float d = 1.0f;
		if (m.path.size() > 1) {
			d = 0.2f;
		}

		if (crow::reached_destination(curr_vel, float2e(curr_pos.x, curr_pos.z),
			m.path.back(), d)) {
			m.path.pop_back();

		}

		result = crow::status::RUNNING;

		/*if (crow::reached_destination({ 0.f, 0.f }, { curr_pos.x, curr_pos.z },
			target_pos, 0.75f)) {
			result = status::PASSED;
		}*/

		//m.print_status("move", result);
		return result;
	}

	// unused && unfinished
	status check_room(float dt, crow::ai_manager& m) { return status(); }

	// interaction branch
	status is_target_door(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;

		if (m.target->type == crow::object_type::DOOR) {
			result = crow::status::PASSED;
		}

		//m.print_status("is_target_door", result);
		return result;
	}

	// TODO: Improve door handling
	status handle_door(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;
		const size_t index = static_cast<size_t>(crow::entity::SPHYNX);
		if (!m.entities->mesh_ptrs[index]->animator->is_acting) { // animtion finished playing
			//// door is closed
			if (m.target->is_active == false) {
				//    // TODO: check logic for closed door as it always seems to destroy the door
				//    // instead of going for another target
				//    unsigned int d_count = 0;
				//    std::vector<crow::interactible*> doors;
				//    for (auto& d : m.curr_room->objects) {
				//        if (d->type == crow::object_type::DOOR) {
				//            ++d_count;
				//            doors.push_back(d);
				//        }
				//    }

				//    // if there is more than one door we will roll to dissable or pick another
				//    if (d_count > 1) {
				//        unsigned roll = static_cast<unsigned int>((rand() % 2));
				//        if (roll == 0) {
				//            m.target->dissable();
				//            m.target->interact(index, *m.entities);
				//            m.prev_target = m.target;
				//            m.target = nullptr;
				//        }
				//        else {
				//            crow::interactible* selected_door = nullptr;

				//            while (!selected_door) {
				//                unsigned int i = static_cast<unsigned int>((rand() % doors.size()));
				//                selected_door = doors[i];
				//            }
				//            m.prev_target = m.target;
				//            m.target = selected_door;
				//        }

				//    }
				//    else {
				//        m.target->dissable();
				//        m.target->interact(index, *m.entities);
				//        m.prev_target = m.target;
				//        m.target = nullptr;
				//    }

				m.target->dissable();
				if (!m.target->is_broken) {
					return crow::status::RUNNING;
				}
				//m.target->interact(index, *m.entities);
			}
			else { // door is open
				m.target->interact(index, *m.entities);
				m.target = nullptr;
				m.interacting = false;
				m.is_roaming = true;
				m.roam_timer = 0.f;
				m.entities->mesh_ptrs[index]->animator->performed_action = false;
				result = crow::status::PASSED;
				/*m.prev_target = m.target;
				m.target = nullptr;*/
			}

			//m.prev_target = m.target;


			m.room_check(); // BEWARE IF YOU DISSABLE THIS IT WILL BREAK THE AI
		}
		else { // animation is playing
			result = crow::status::RUNNING;
		}

		//m.print_status("handle_door", result);
		return result;
	}

	status destroy_target(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;
		size_t index = static_cast<size_t>(crow::entity::SPHYNX);

		if (m.entities->mesh_ptrs[index]->animator && !m.entities->mesh_ptrs[index]->animator->is_acting) {

			if (m.target->type == crow::object_type::PLAYER && m.debug_mode) {
				m.target = nullptr;
				m.counter--;
				m.interacting = false;
				m.entities->mesh_ptrs[index]->animator->performed_action = false;
				result = crow::status::PASSED;
			}
			else {
				m.target->dissable();
				m.target = nullptr;
				m.counter--;
				m.interacting = false;
				m.entities->mesh_ptrs[index]->animator->performed_action = false;
				result = crow::status::PASSED;
			}
		}
		else {
			result = crow::status::RUNNING;
		}

		//m.print_status("\ndestroy_target", result);
		return result;
	}

	void ai_manager::room_check() {
		bool finished = false;
		for (auto& i : curr_level->rooms) {
			for (auto& j : i) {
				if (j.has_ai) {
					finished = true;
					curr_room = &j;
					break;
				}
			}
			if (finished) {
				break;
			}
		}
	}

	void ai_manager::init_manager(crow::entities* ent, crow::level* level) {
		entities = ent;
		curr_level = level;
		this->room_check();
		srand(time(0));
	}

	void ai_manager::print_status(std::string behavior, status b_status)
	{
		std::string r = "";
		switch (b_status)
		{
		case crow::status::FAILED: {
			r = ": FAILED\n";
			break;
		}
		case crow::status::PASSED: {
			r = ": PASSED\n";
			break;
		}
		case crow::status::RUNNING: {
			r = ": RUNNING\n";
			break;
		}
		default:
			break;
		}
		r = behavior + r;
		std::printf(r.c_str());
	}

}  // namespace crow