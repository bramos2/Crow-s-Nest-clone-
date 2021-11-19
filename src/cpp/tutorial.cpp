#include "../hpp/game_manager.hpp"

namespace crow {
	void game_manager::t_first_control_message() {
		c_buffered_message.set(message("Left click anywhere on the map to move.", 4.0f), 1.25, &game_manager::t_second_control_message);
	}
	
	void game_manager::t_second_control_message() {
		c_buffered_message.set(message("Click on objects to interact with them.", 4.0f), 4.5, &game_manager::t_third_control_message);
	}

	void game_manager::t_third_control_message() {
		// tell the player about room changing if they are still here, give a different message otherwise
		if (current_level.rooms[2][0].has_player)
			c_buffered_message.set(message("Click on doors to change rooms.", 4.0f), 4.5, &game_manager::t_fourth_control_message);
		else
			c_buffered_message.set(message("Right click on the minimap to change views.", 4.0f), 4.5, nullptr);
	}
	
	void game_manager::t_fourth_control_message() {
		c_buffered_message.set(message("Right click on the minimap to change views.", 4.0f), 4.5, nullptr);
	}
	
	void game_manager::t_fifth_control_message() {
		c_buffered_message.set(message("You can also move the minimap with right click + drag.", 4.0f), 4.5, nullptr);
	}
	
	void game_manager::t_locked_door_message() {
		current_message = message("Locked doors will have a red light.", 4.0f);

		c_buffered_message.set(message("Interact with their respective panels to unlock them.", 4.0f), 4.5, &game_manager::t_fifth_control_message);
	}
	
	void game_manager::t_scrollwheel_message() {
		c_buffered_message.set(message("You can zoom the minimap with the mouse wheel.", 4.0f), 0.5f, nullptr);
	}
	
	void game_manager::t_pressure_message2() {
		c_buffered_message.set(message("Locate it and interact to repair, or the worker will die.", 4.0f), 0.02f, nullptr);
	}
}
