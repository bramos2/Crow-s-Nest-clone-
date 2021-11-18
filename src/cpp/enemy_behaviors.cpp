#include "..\hpp\enemy_behaviors.hpp"
#include "../hpp/audio.hpp"

#include <time.h>

#include "../hpp/player_behavior.hpp"

namespace crow {

	status passive_roam_check(float dt, crow::ai_manager& m)
	{
		m.room_check();
		status result = crow::status::FAILED;
		m.roam_timer += dt;
		if (m.roam_timer >= m.roam_total || m.curr_room->has_ai) {
			m.is_roaming = false;
		}

		if (m.is_roaming) {
			result = crow::status::PASSED;
		}

		return result;
	}

	status passive_has_target(float dt, crow::ai_manager& m)
	{
		status result = crow::status::FAILED;
		// TODO: CHANGE CHECK TO AVOID ENEMY AI
		if (m.target && m.target->roomptr == m.curr_room) {
			result = crow::status::PASSED;
		}
		else {
			m.target = nullptr;
			m.interacting = false;
		}

		return result;
	}

	status roam_check(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;
		m.roam_timer += dt;
		if (m.roam_timer >= m.roam_total) {
			m.is_roaming = false;
		}

		if (!m.curr_room->live_entities.empty()) {
			for (auto& indx : m.curr_room->live_entities) {
				if ((*m.live_units)[indx]->is_active) {
					m.is_roaming = false;
					break;
				}
			}
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
				m.index);
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
		// check for a static object ex: door, console
		if (m.target) {
			const crow::object_type curr_type = m.target->type;
			if (curr_type != crow::object_type::PLAYER && curr_type != crow::object_type::AI && m.curr_room->live_entities.empty()) {
				if (m.target->roomptr == m.curr_room) {
					result = crow::status::PASSED;
				}
				else {
					m.target = nullptr;
					m.interacting = false;
				}
				// check for non static object ex: player, npc
			}
			else if ((curr_type == crow::object_type::PLAYER || curr_type == crow::object_type::AI) && !m.curr_room->live_entities.empty()) {
				for (auto& t : m.curr_room->live_entities) {
					// checking if the entity is in the current room
					if ((*m.live_units)[t] == m.target) {
						result = crow::status::PASSED;
						break;
					}
				}
			}
		}
		//m.print_status("has_target", result);
		return result;
	}

	status target_player(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;
		// we need to check if the player is in this room
		if (!m.curr_room || !m.live_units || m.curr_room->live_entities.empty()) {
			return result;
		}

		crow::interactible* temp_target = nullptr;
		if (m.curr_room->live_entities.size() > 1) {
			const int tc = m.curr_room->live_entities[rand() % m.curr_room->live_entities.size()];
			temp_target = (*m.live_units)[tc];
		}
		else if (m.curr_room->live_entities.size() == 1) {
			temp_target = (*m.live_units)[m.curr_room->live_entities.back()];
		}
		// todo: remove p_inter check
		if (temp_target && temp_target->is_active) {
			m.target = temp_target;
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
			if (!room_doors[i]->is_active && !m.friendly) {
				int num = rand() % 100 + 1;
				if (num <= 50) {
					add_door = true;
				}
			}
			else if (room_doors[i]->is_active) {
				add_door = true;
			}

			if (!add_door) { continue; }

			//friendly AI always adds the door
			if (tinx.empty() || m.friendly) {
				tinx.push_back(i);
				continue;
			}

			// enemy AI will track entities using heat values
			if (room_doors[i]->heat > room_doors[tinx.back()]->heat) {
				// if the next door has a higher heat than the doors already in the list, clear the list and add new door

				tinx.clear();
				tinx.push_back(i);
			}
			else if (room_doors[i]->heat == room_doors[tinx.back()]->heat) {
				// otherwise if the door has the same heat, add it to the list
				tinx.push_back(i);
			}
			// we ignore any doors with lower heat values

		}

		// i ran into a divide by zero error here,
		// this line should prevent the game from crashing,
		// but i don't know what will happen when tinx.size() == 0
		if (tinx.size() == 0) return result;

		// we pick a random index inside the index list
		size_t choice = rand() % tinx.size();

		// the target is set and the test passes
		result = crow::status::PASSED;
		m.target = room_doors[tinx[choice]];

		//m.print_status("target_door", result);
		return result;
	}

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
		// we need to check if this path is set to the current target, otherwise we
		// must clean it and get a new one
		// non moving target, use its tile
		float3e curr_pos = m.entities->get_world_position(m.index);
		if (m.path.empty() && m.interacting) {

			float2e target_pos;
			if (m.target->type == crow::object_type::PLAYER || m.target->type == crow::object_type::AI) {
				// TODO: store entity index in the interactible for modularity
				float3e p_pos = m.entities->get_world_position(
					m.target->entity_index);
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
			if (m.target->type != crow::object_type::PLAYER && m.target->type != crow::object_type::AI) {
				//float2e temp = m.curr_room->get_tile_wpos(m.target->x, m.target->y);

				/*if (temp == m.path[0]) {
					result = crow::status::PASSED;
				}*/
				result = crow::status::PASSED;
			}
			else {  // for player use entities
				//TODO: SAME AS ABOVE
				float3e temp = m.entities->get_world_position(
					m.target->entity_index);
				float2e t = { temp.x, temp.z };
				// changing the last value will determine how much the player has to move from its initial position before the AI makes a different path
				if (crow::reached_destination({ 0.f, 0.f }, t, m.path[0], 3.0f)) {
					result = crow::status::PASSED;
				}
			}
		}

		//m.print_status("is_path_current", result);
		return result;
	}

	status get_path(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;

		m.path.clear();
		float3e temp_ai_pos =
			m.entities->get_world_position(m.index);

		float2e target_pos;
		if (m.target->type == crow::object_type::PLAYER || m.target->type == crow::object_type::AI) {
			//TODO: USE ENTITY INDEX FROM TARGET INSTEAD
			float3e p_pos = m.entities->get_world_position(m.target->entity_index);
			target_pos = { p_pos.x, p_pos.z };
		}
		else {
			const crow::tile* ai_tile =
				m.curr_room->get_tile_at({ temp_ai_pos.x, temp_ai_pos.z });

			if (!ai_tile || !ai_tile->is_open) {
				return crow::status::FAILED;
			}

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

		m.path = m.curr_room->get_path(float2e(temp_ai_pos.x, temp_ai_pos.z),
			target_pos);

		m.interacting = true;
		result = crow::status::PASSED;

		//m.print_status("get_path", result);
		return result;
	}

	// move branch (bugged, passing when target enters new room and player is there)
	status reached_target(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;

		/*float3e curr_pos = m.entities->get_world_position(index);
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
			result = crow::status::PASSED;
			m.entities->velocities[m.index] = {
				0.f, 0.f, 0.f };
			if (!m.friendly && !m.entities->mesh_ptrs[m.index]->animator->is_acting && !m.entities->mesh_ptrs[m.index]->animator->performed_action) {
				m.entities->mesh_ptrs[m.index]->animator->switch_animation(1);
				m.entities->mesh_ptrs[m.index]->animator->performed_action = true;
				//std::printf("\nAI: playing action animation");
			}
		}

		//m.print_status("reached_target", result);
		return result;
	}

	status move(float dt, crow::ai_manager& m) {
		status result = crow::status::FAILED;

		crow::set_velocity(m.path.back(), *m.entities, m.index, m.roam_speed);
		float3e curr_pos = m.entities->get_world_position(m.index);

		float2e curr_vel = float2e(dt * m.entities->velocities[m.index].x,
			dt * m.entities->velocities[m.index].z);
		float d = 1.0f;
		if (m.path.size() > 1) {
			d = 0.5f;
		}

		if (crow::reached_destination(curr_vel, float2e(curr_pos.x, curr_pos.z),
			m.path.back(), d)) {
			m.path.pop_back();
		}

		result = crow::status::RUNNING;

		//m.print_status("move", result);
		return result;
	}

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
		if (!m.entities->mesh_ptrs[m.index]->animator->is_acting) { // animtion finished playing
			//// door is closed
			if (m.target->is_active == false) {
				// sound effect of destroyed target
				crow::audio::play_sfx(crow::audio::SFX::ENEMY_ATTACK);

				m.target->dissable();
				if (!m.target->is_broken) {
					return crow::status::RUNNING;
				}
			}
			else { // door is open
				m.target->interact(m.index, *m.entities, m.inter_index);
				m.target = nullptr;
				m.interacting = false;
				m.is_roaming = true;
				m.roam_timer = 0.f;
				m.entities->mesh_ptrs[m.index]->animator->performed_action = false;
				result = crow::status::PASSED;
			}

			m.entities->mesh_ptrs[m.index]->animator->switch_animation(0);
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

		if (m.entities->mesh_ptrs[m.index]->animator && !m.entities->mesh_ptrs[m.index]->animator->is_acting) {

			if (m.target->type == crow::object_type::PLAYER && m.debug_mode) {
				m.target = nullptr;
				m.counter--;
				m.interacting = false;
				m.entities->mesh_ptrs[m.index]->animator->performed_action = false;
				result = crow::status::PASSED;
			}
			else {
				// sound effect of destroyed target
				crow::audio::play_sfx(crow::audio::SFX::ENEMY_ATTACK);

				m.target->dissable();
				m.target = nullptr;
				m.counter--;
				m.interacting = false;
				m.entities->mesh_ptrs[m.index]->animator->performed_action = false;
				result = crow::status::PASSED;
			}
			m.entities->mesh_ptrs[m.index]->animator->switch_animation(0);
		}
		else {
			result = crow::status::RUNNING;
		}

		//m.print_status("\ndestroy_target", result);
		return result;
	}

	void ai_manager::room_check() {
		for (auto& i : curr_level->rooms) {
			for (auto& j : i) {
				if (index == crow::entity::SPHYNX) {
					if (j.has_ai) {
						curr_room = &j;
						return;
					}
				}
				else {
					for (auto& unit : j.live_entities) {
						if (unit == inter_index) {
							curr_room = &j;
							return;
						}
					}
				}
			}
		}
	}

	void ai_manager::init_manager(crow::entities* ent, crow::level* level, std::vector<crow::interactible*>* l_units) {
		entities = ent;
		curr_level = level;
		live_units = l_units;
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