#include "../hpp/interactible.hpp"
#include "../hpp/audio.hpp"

#include "../hpp/game_manager.hpp"
#include "../hpp/map.hpp"

namespace crow {

	void interactible::interact(size_t const index, crow::entities& entity, int inter_index) {
		if (is_broken) {
			is_broken = false;
		}
		is_active = !is_active;
	}

	void interactible::activate(crow::game_manager& state) {/* printf("something was activated.\n");*/ }

	void interactible::dissable() {
		is_broken = true;
		is_active = false;
		//if (health <= 0) {
		//	is_broken = true;
		//	//is_active = false;
		//}
		//else {
		//	--health;
		//}
	}

	void interactible::set_tile(unsigned int _x, unsigned int _y) {
		x = _x;
		y = _y;
	}

	void interactible::set_tile(char dir) {
		switch (dir) {
		case 'u': x = 12; y = 14; break;
		case 'l': x = 0; y = 7; break;
		case 'd': x = 12; y = 0; break;
		case 'r': x = 24; y = 7; break;
		}
	}

	interactible::interactible(unsigned int _x, unsigned int _y) : x(_x), y(_y) {}

	// self destruct console
	void sd_console::interact(size_t const index, crow::entities& entity, int inter_index) {
		if (power != nullptr && (power->is_broken)) {
			current_level->msg = message("This console needs the power on to be used.");

			return;
		}


		if (is_active) {
			current_level->msg = message("SELF DESTRUCT SEQUENCE ALREADY ACTIVE. EVACUATE IMMEDIATELY!");
		}
		else {
			current_level->interacting = this;
			current_level->msg = message("Activating self-destruct sequence...", crow::default_message_time - 1.0f,
				crow::default_interact_wait);
			// TODO::we can use the return from this function to stop the sound if the action is cancelled
			int temp = crow::audio::play_sfx(crow::audio::SFX::INTERACT);
		}
	}

	void sd_console::activate(crow::game_manager& state) {
		is_broken = false;
		is_active = true;
		state.self_destruct_timer = 180;
		state.good_ending = true;

		// self-destruct song
		crow::audio::play_bgm(crow::audio::BGM::ESCAPE);
		crow::audio::play_sfx(crow::audio::SFX::ALARM);
		// uh-oh we gon' splode
		state.current_message = message("SELF DESTRUCT SEQUENCE ACTIVATED. EVACUATE IMMEDIATELY!");
	}

	sd_console::sd_console(crow::level* _lv) {
		current_level = _lv;
		type = crow::object_type::SD_CONSOLE;
		is_active = false;
		health = 99;
	}

	void sd_console::dissable() {
		/* THE SD CONSOLE IS TOO POWERFUL TO BE DISSABLED */
	}

	void pg_console::activate(crow::game_manager& state) {
		is_broken = false;
	}

	// power generator console
	void pg_console::interact(size_t const index, crow::entities& entity, int inter_index) {
		if (!is_broken) {
			current_level->msg = message("The power is already working!");
		}
		else {
			current_level->interacting = this;
			current_level->msg = message("Restoring power grid...", crow::default_message_time - 1.0f,
				crow::default_interact_wait);
			// TODO::we can use the return from this function to stop the sound if the action is cancelled
			int temp = crow::audio::play_sfx(crow::audio::SFX::INTERACT);
		}
	}

	pg_console::pg_console(crow::level* _lv) {
		current_level = _lv;
		type = crow::object_type::POWER_CONSOLE;
		_lv->power_console = this;
	}

	// oxygen console
	oxygen_console::oxygen_console(crow::level* _lv) {
		type = crow::object_type::OXYGEN_CONSOLE;
		current_level = _lv;
		is_broken = true;
	}

	void oxygen_console::interact(size_t const index, crow::entities& entity, int inter_index) {
		if (!is_broken) {
			current_level->msg = message("The oxygen console is already repaired!");
		}
		else {
			current_level->interacting = this;
			current_level->msg = message("Repairing oxygen console...", crow::default_message_time - 1.0f,
				crow::default_interact_wait);
			// TODO::we can use the return from this function to stop the sound if the action is cancelled
			int temp = crow::audio::play_sfx(crow::audio::SFX::INTERACT);
		}
	}

	void oxygen_console::activate(crow::game_manager& state) {
		is_broken = false;
	}

	// preassure console
	pressure_console::pressure_console(crow::level* _lv) {
		type = crow::object_type::PRESSURE_CONSOLE;
		current_level = _lv;
		is_broken = false;
		health = 1;
	}

	void pressure_console::interact(size_t const index, crow::entities& entity, int inter_index) {
		if (!is_broken) {
			current_level->msg = message("The pressure console is already repaired!");
		}
		else {
			current_level->interacting = this;
			current_level->msg = message("Repairing pressure console...", crow::default_message_time - 1.0f,
				crow::default_interact_wait);
			// TODO::we can use the return from this function to stop the sound if the action is cancelled
			int temp = crow::audio::play_sfx(crow::audio::SFX::INTERACT);
		}
	}

	void pressure_console::activate(crow::game_manager& state) {
		is_broken = false;
	}

	void pressure_console::dissable()
	{
		is_broken = true;
		pressure = pressure_max;
	}

	// door panel
	void door_panel::interact(size_t const index, crow::entities& entity, int inter_index) {
		// we should only do any of this is the door is not broken
		if (door->is_broken) {
			current_level->msg = message("DOOR IS BROKEN, CANNOT LOCK...");
			return;
		}

		// this will prevent the game from crashing if the door is improperly configured
		if (!door->neighbor) {
			printf("error! the door attached to the panel doesn't have a neighbor attached!\n");
			return;
		}

		std::string text;
		// We don't need anything else, stop it, get some help
		if (door->is_active) {
			text = "LOCKING...";
		}
		else {
			text = "UNLOCKING...";
		}

		current_level->interacting = this;
		current_level->msg = message(text, crow::default_message_time - 1.0f,
			crow::default_interact_wait);
		// TODO::we can use the return from this function to stop the sound if the action is cancelled
		int temp = crow::audio::play_sfx(crow::audio::SFX::INTERACT);
	}

	// we may not need to pass in the game state anymore
	void door_panel::activate(crow::game_manager& state) {
		door->is_active = door->neighbor->is_active = !door->is_active;
		if (door->is_active) {
			crow::audio::play_sfx(crow::audio::SFX::DOOR_UNLOCK);
		}
		else {
			crow::audio::play_sfx(crow::audio::SFX::DOOR_LOCK);
		}
	}

	door_panel::door_panel(crow::level* _lv) {
		type = crow::object_type::DOOR_PANEL;
		current_level = _lv;
	}
	
	door_panel::door_panel(crow::level* _lv, char _tile, crow::door* _door, float2e offset) {
		type = crow::object_type::DOOR_PANEL;
		current_level = _lv;
		door = _door;

		set_tile(_tile);
		x += offset.x;
		y += offset.y;
	}


	// door
	void door::interact(size_t const index, crow::entities& entity, int inter_index) {
		// we will be moving the index out of this room
		if (!roomptr || !neighbor) { // if this returns the doors was not created properly during level creation
			std::printf("error! the door is not properly configured!");
			return;
		}

		if (is_active == false) {
			crow::audio::play_sfx(crow::audio::SFX::DOOR_LOCKED);
			return;
		}

		// to move an entity from a room to another we just remove its index from the current room and push it into the neighbor's room
		// checking the room indices
		if (!roomptr->object_indices.empty()) {

			// moving the interactible index to back to remove
			if (!roomptr->live_entities.empty() && inter_index >= 0) {
				if (roomptr->live_entities.back() != inter_index) {
					const int temp_indx = roomptr->live_entities.back();

					for (auto& i : roomptr->live_entities) {
						if (i == inter_index) {
							i = temp_indx;
							break;
						}
					}
					roomptr->live_entities.back() = inter_index;
				}
				roomptr->live_entities.pop_back();
				neighbor->roomptr->live_entities.push_back(inter_index);
			}
			

			// we are going to move the current entity's index out of this room and to the neighbor's room
			if (roomptr->object_indices.back() != index) {
				// swapping the current back with the entity's index for easy removal
				const size_t temp = roomptr->object_indices.back();

				for (auto& i : roomptr->object_indices) {
					if (i == index) {
						i = temp;
						break;
					}
				}
				roomptr->object_indices.back() = index;
			}
			// removing the entity's index from this room
			roomptr->object_indices.pop_back();

			// adding the entity's index into the neighbor's room
			neighbor->roomptr->object_indices.push_back(index);
			// setting the position of the entity to be the neighbor's door
			float2e npos = neighbor->roomptr->get_tile_wpos(neighbor->x, neighbor->y);
			entity.set_world_position(index, npos.x, 0.f, npos.y);

			// this portion handles the heat system the AI uses to track the player
			// TODO: rebalance
			if (index == static_cast<size_t>(crow::entity::SPHYNX)) {
				roomptr->has_ai = false;
				neighbor->roomptr->has_ai = true;
				heat = neighbor->heat = -2.f;
			}
			else if (index == static_cast<size_t>(crow::entity::WORKER) ){
				roomptr->has_player = false;
				neighbor->roomptr->has_player = true;
				heat = neighbor->heat = 2.f;
			} else {
				heat = neighbor->heat = 2.f;
			}
		}
	}

	// when the AI dissables a door it will break it open
	void door::dissable()
	{
		if (health <= 0) {
			is_active = true;
			is_broken = true;
			neighbor->is_active = true;
			neighbor->is_broken = true;
		}
		else {
			--health;
		}


	}

	door::door() {
		type = crow::object_type::DOOR;
		is_active = true;
		is_broken = false;
	}

	door::door(bool open)
	{
		type = crow::object_type::DOOR;
		is_active = open;
		is_broken = false;
	}

	door::door(crow::level* _lv, bool open) {
		type = crow::object_type::DOOR;
		is_active = open;
		is_broken = false;
		current_level = _lv;
	}

	door::door(crow::level* _lv, bool _open, char _tile, crow::room* _roomptr, crow::door* _neighbor) {
		type = crow::object_type::DOOR;
		is_active = _open;
		is_broken = false;
		current_level = _lv;
		roomptr = _roomptr;
		neighbor = _neighbor;
		if (neighbor) neighbor->neighbor = this;
		set_tile(_tile);
		roomptr->objects.push_back(this);
	}

	door::door(crow::level* _lv, bool _open, float2e _tile, crow::room* _roomptr, crow::door* _neighbor) {
		type = crow::object_type::DOOR;
		is_active = _open;
		is_broken = false;
		current_level = _lv;
		roomptr = _roomptr;
		neighbor = _neighbor;
		if (neighbor) neighbor->neighbor = this;
		x = _tile.x;
		y = _tile.y;
		roomptr->objects.push_back(this);
	}

	// exit door
	void exit::interact(size_t const index, crow::entities& entity, int inter_index) {
		if (power != nullptr && (power->is_broken)) {
			current_level->msg = message("The door won't open because it has no power.");

			return;
		}

		//std::printf("\ninteracted with exit, congrats! loaded level: %i",
		//	level_num + 1);
		state->change_level(level_num + 1);
		// nothing should be called after the change level command
	}

	exit::exit(game_manager* _state, crow::level* _lv, int _level_num) {
		type = crow::object_type::EXIT;
		current_level = _lv;
		state = _state;
		level_num = _level_num;
	}

	// player
	void player_interact::dissable() {
		//std::printf("\n*****enemy is attacking player\n");
		//interactible::dissable();
		is_active = false;
		is_broken = true;
	}

	player_interact::player_interact() {
		type = crow::object_type::PLAYER;
		is_active = true;
		entity_index = crow::entity::WORKER;
	}

	void m_door_panel::interact(size_t const index, crow::entities& entity, int inter_index) {
		std::string text;
		int broken = -1;

		for (auto& door : doors) {
			if (door->is_broken) {
				current_level->msg = message("DOOR IS BROKEN, CANNOT LOCK...");
				if (broken != 0) broken = 1;
				continue;
			}

			// this will prevent the game from crashing if the door is improperly configured
			if (!door->neighbor) {
				printf("error! a door attached to the panel doesn't have a neighbor attached!\n");
				continue;
			}

			if (!door->is_active) {
				text = "UNLOCKING...";
				broken = 0;
			} else if (!text.size()) {
				text = "LOCKING...";
				broken = 0;
			}
		}

		if (broken == 0) {
			current_level->interacting = this;
			current_level->msg = message(text, crow::default_message_time - 1.0f,
				crow::default_interact_wait);
		}
		// TODO::we can use the return from this function to stop the sound if the action is cancelled
		int temp = crow::audio::play_sfx(crow::audio::SFX::INTERACT);
	}

	void m_door_panel::activate(crow::game_manager& state) {
		int sound = 0;

		for (auto& door : doors) {
			// don't activate broken doors
			if (door->is_broken) continue;

			// toggle the door state
			door->is_active = door->neighbor->is_active = !door->is_active;

			// buffer the sound (we don't want to play a sound per door locked/unlocked)
			if (door->is_active) {
				sound = 1;
			} else {
				if (!sound) sound = 2;
			}
		}

		if (sound == 1) {
			crow::audio::play_sfx(crow::audio::SFX::DOOR_UNLOCK);
		} else if (sound == 2) {
			crow::audio::play_sfx(crow::audio::SFX::DOOR_LOCK);
		}
	}

	m_door_panel::m_door_panel(crow::level* _lv) {
		type = crow::object_type::DOOR_PANEL_V2;
		current_level = _lv;
	}
	
	m_door_panel::m_door_panel(crow::level* _lv, char _tile, float2e offset) {
		type = crow::object_type::DOOR_PANEL_V2;
		current_level = _lv;

		set_tile(_tile);
		x += offset.x;
		y += offset.y;
	}
}  // namespace crow
