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
		is_active = false;
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

	pressure_console::pressure_console(crow::level* _lv) {
		type = crow::object_type::OXYGEN_CONSOLE;
		current_level = _lv;
		is_broken = true;
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
    switch (panel_type) {
        // hackable door
    case 0:
        // if the door is unusable, make it useable. otherwise, toggle between
        // open and closed.
        switch (panel_status) {
        case 0:
            text = "ACTIVATING...";
            break;
        case 1:
            text = "LOCKING...";
            break;
        case 2:
            text = "UNLOCKING...";
            break;
        }
        break;
        // repairable door
    case 1:
        if (panel_status == 1) {
        current_level->msg = message("This door panel is already repaired!");
        return;
        } else
        text = "REPAIRING...";
        break;
    }
    current_level->interacting = this;
    current_level->msg = message(text, crow::default_message_time - 1.0f,
                                crow::default_interact_wait);
}

void door_panel::activate(crow::game_manager& state) {
    switch (panel_type) {
        // hackable door
    case 0:
        // if the door is unusable, make it useable. otherwise, toggle between
        // open and closed.
        switch (panel_status) {
        case 0:
            panel_status = 1;
            door->is_active = door->neighbor->is_active = true;
            break;
        case 1:
            panel_status = 2;
            door->is_active = door->neighbor->is_active = false;
            break;
        case 2:
            panel_status = 1;
            door->is_active = door->neighbor->is_active = true;
            break;
        }
        break;
    case 1:
        // repairable doors can be repaired only. there is nothing to do to it
        // other than repair it. it shouldn't be able to be activated if it's
        // already repaired, but if it does get activated nothing will happen
        panel_status = 1;
        door->is_active = door->neighbor->is_active = true;
        break;
    }
    
    if (!door->is_active) {
        state.entities.s_resource_view[door->entity_index] = state.textures[game_manager::texture_list::DOOR_CLOSED];
    } else {
        state.entities.s_resource_view[door->entity_index] = state.textures[game_manager::texture_list::DOOR_OPEN];
    }

    if (!door->neighbor->is_active) {
        state.entities.s_resource_view[door->neighbor->entity_index] = state.textures[game_manager::texture_list::DOOR_CLOSED];
    } else {
        state.entities.s_resource_view[door->neighbor->entity_index] = state.textures[game_manager::texture_list::DOOR_OPEN];
    }
}

	door_panel::door_panel(crow::level* _lv) {
		type = crow::object_type::DOOR_PANEL;
		current_level = _lv;
	}

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

	void player_interact::dissable() {
		std::printf("\n*****enemy is attacking player\n");
		interactible::dissable();
	}

	player_interact::player_interact() {
		type = crow::object_type::PLAYER;
		is_active = true;
	}

	void door::interact(size_t const index, crow::entities& entity) {
		std::printf("\ninteracted with door, congrats!");

		// This portion has been deprecated as it doesn't changed the proper values, also we are simplifying the door
		// panel to look at
		// if this door doesn't have a panel attached, it will check to see if the
		// neighboring door has a panel this is the reason the neigbor is never locked
		//crow::door_panel* _panel = panel;
		//if (!_panel) _panel = neighbor->panel;

		//// notifs are passed up via a "bubble up" method
		//if (_panel != nullptr) {
		//  // door is unuseable, open up!
		//  if (_panel->panel_status == 0) {
		//    if (current_level) {
		//      // door can't open, give message to warn that the door is unusable
		//      current_level->msg = message(
		//          "The door is broken and needs to be repaired before it can be "
		//          "used.");
		//      if (_panel->panel_type == 0) {
		//        current_level->msg = message(
		//            "The door isn't working and needs to be reprogrammed before it "
		//            "can be used.");
		//      }
		//    } else {
		//      std::printf(
		//          "\nerror! tried to display a message, but the door doesn't have a "
		//          "reference to the current level!");
		//    }

		//    return;
		//    // door has been hacked shut by the player. give message to alert the
		//    // player of this.
		//  } else if (_panel->panel_status == 2) {
		//    if (current_level) {
		//      current_level->msg = message(
		//          "You've hacked this door shut. You need to unlock it before you "
		//          "can use it.");
		//    } else {
		//      std::printf(
		//          "\nerror! tried to display a message, but the door doesn't have a "
		//          "reference to the current level!");
		//    }
		//    return;
		//  }
		//}

		// we will be moving the index out of this room
		if (!roomptr || !neighbor) { // if this returns the doors was not created properly during level creation
			return;
		}

		if (!this->is_active) { // if the door is closed, dont do anything and just send a message... otherwise handle entity index swapping
			current_level->msg = message("door is locked");
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
				heat++;
				neighbor->heat++;
			}
			else {
				roomptr->has_ai = false;
				neighbor->roomptr->has_ai = true;
				heat--;
				neighbor->heat--;
			}
		}


	}

	void door::dissable()
	{
		is_active = true;
		is_broken = true;
		neighbor->is_active = true;
		neighbor->is_broken = true;
	}

	door::door() {
		type = crow::object_type::DOOR;
		is_active = true;
		is_broken = false;
	}

	door::door(crow::level* _lv) {
		type = crow::object_type::DOOR;
		is_active = true;
		is_broken = false;
		current_level = _lv;
	}

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

}  // namespace crow
