#include "../hpp/interactible.hpp"

#include "../hpp/game_manager.hpp"
#include "../hpp/map.hpp"

namespace crow {

	void interactible::interact(size_t const index, crow::entities& entity) {
		if (!is_broken) {
			std::printf("\n*****interacted with interactible*****\n");
		}
		else {
			std::printf("\n*****fixing interactible*****\n");
			is_broken = false;
		}
		is_active = !is_active;

		if (is_active) {
			std::printf("*****power generator online*****");
		}
		else {
			std::printf("*****power generator offline*****");
		}
	}

	void interactible::activate(crow::game_manager& state) { printf("something was activated.\n"); }

	void interactible::dissable() {
		std::printf("\n*****interactible has been destroyed*****");
		is_broken = true;
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
	void sd_console::interact(size_t const index, crow::entities& entity) {
		if (!is_broken) {
			std::printf("\n*****interacted with sd_console*****\n");
		}
		else {
			std::printf("\n*****fixing sd_console*****\n");
			is_broken = false;
		}
		is_active = true;
	}

	sd_console::sd_console() { type = crow::object_type::SD_CONSOLE; }

	// power generator console
	void pg_console::interact(size_t const index, crow::entities& entity) {
		if (!is_broken) {
			std::printf("\n*****interacted with pg_console*****\n");
		}
		else {
			std::printf("\n*****fixing pg_console*****\n");
			is_broken = false;
		}
		is_active = true;
	}

	pg_console::pg_console() { type = crow::object_type::POWER_CONSOLE; }

	// oxygen console
	oxygen_console::oxygen_console(crow::level* _lv) {
		type = crow::object_type::OXYGEN_CONSOLE;
		current_level = _lv;
		is_broken = true;
	}

	void oxygen_console::interact(size_t const index, crow::entities& entity) {
		if (!is_broken) {
			current_level->msg = message("The oxygen console is already repaired!");
		}
		else {
			current_level->interacting = this;
			current_level->msg = message("Repairing oxygen console...", crow::default_message_time - 1.0f,
				crow::default_interact_wait);
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

	void pressure_console::interact(size_t const index, crow::entities& entity) {
		if (!is_broken) {
			current_level->msg = message("The pressure console is already repaired!");
		}
		else {
			current_level->interacting = this;
			current_level->msg = message("Repairing pressure console...", crow::default_message_time - 1.0f,
				crow::default_interact_wait);
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
	void door_panel::interact(size_t const index, crow::entities& entity) {
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
	}

	// we may not need to pass in the game state anymore
	void door_panel::activate(crow::game_manager& state) {
		door->is_active = door->neighbor->is_active = !door->is_active;
	}

	door_panel::door_panel(crow::level* _lv) {
		type = crow::object_type::DOOR_PANEL;
		current_level = _lv;
	}

	// door
	void door::interact(size_t const index, crow::entities& entity) {
		//std::printf("\ninteracted with door, congrats!");

		// we will be moving the index out of this room
		if (!roomptr || !neighbor) { // if this returns the doors was not created properly during level creation
			std::printf("error! the door is not properly configured!");
			return;
		}

		if (is_active == false) {
			// we should play some sort of sound here maybe, the door is visually locked so no point on writting a message
			return;
		}

		// to move an entity from a room to another we just remove its index from the current room and push it into the neighbor's room
		// checking the room indices
		if (!roomptr->object_indices.empty()) {
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
			if (index == static_cast<size_t>(crow::entity::WORKER)) {
				roomptr->has_player = false;
				neighbor->roomptr->has_player = true;
				heat = neighbor->heat = 2.f;
			}
			else {
				roomptr->has_ai = false;
				neighbor->roomptr->has_ai = true;
				heat = neighbor->heat = -2.f;
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

	// exit door
	void exit::interact(size_t const index, crow::entities& entity) {
		std::printf("\ninteracted with exit, congrats! loaded level: %i",
			level_num + 1);
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
		std::printf("\n*****enemy is attacking player\n");
		//interactible::dissable();
		is_active = false;
		is_broken = true;
	}

	player_interact::player_interact() {
		type = crow::object_type::PLAYER;
		is_active = true;
	}
}  // namespace crow
