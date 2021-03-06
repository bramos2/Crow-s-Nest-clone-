#pragma once

#include <iostream>

#include "../hpp/entities.hpp"
#include "../hpp/message.hpp"

namespace crow {
	// forward declares for circular includes
	class game_manager;
	struct room;
	struct level;
	//struct door_panel;

	enum class object_type {
		PLAYER = 0,
		ENEMY = 1,
		SD_CONSOLE,
		POWER_CONSOLE,
		DOOR,
		EXIT,
		DOOR_PANEL,
		DOOR_PANEL_V2,
		OXYGEN_CONSOLE,
		PRESSURE_CONSOLE,
		FLOOR,
		WALL,
		SHORT_WALL,
		AI,
		COUNT,
	};

	// when current_level == final_level, we will skip to the win screen instead of
	// attempting to advance to the next level
	static constexpr int final_level = 5;

	struct interactible {
		virtual void interact(size_t const index, crow::entities& entity, int inter_index = -1);
		// similar to interact(), but for objects that require time to activate
		virtual void activate(crow::game_manager& state);
		virtual void dissable();
		void set_tile(unsigned int _x, unsigned int _y);
		void set_tile(char dir);
		// x and y coordinates in the tile map for the room that contains the item
		unsigned int x = 0;
		unsigned int y = 0;
		bool is_active = false;
		bool is_broken = false;
		int health = 2;
		float heat = 0;
		// location of this object in the entities system, assigned when meshes are generated
		int entity_index = -1;
		crow::object_type type = crow::object_type::COUNT;
		// MUST BE SET
		crow::room* roomptr = nullptr;
		crow::level* current_level = nullptr;
		interactible(unsigned int x, unsigned int y);
		interactible() = default;
	};

	struct pg_console : public interactible {
		// TODO: gives power to a level
		virtual void interact(size_t const index, crow::entities& entity, int inter_index = -1);
		virtual void activate(crow::game_manager& state);
		pg_console(crow::level* _lv);
	};

	struct oxygen_console : public interactible {
		// TODO: gives power to a level
		virtual void interact(size_t const index, crow::entities& entity, int inter_index = -1);
		virtual void activate(crow::game_manager& state);
		oxygen_console(crow::level* _lv);
	};

	struct pressure_console : public interactible {
		// TODO: gives power to a level
		float pressure = 230.f;
		const float pressure_max = 230.f;
		virtual void interact(size_t const index, crow::entities& entity, int inter_index = -1);
		virtual void activate(crow::game_manager& state);
		virtual void dissable();
		pressure_console(crow::level* _lv);
	};

	struct door : public interactible {
		// TODO: Make player transition
		virtual void interact(size_t const index, crow::entities& entity, int inter_index = -1);
		virtual void dissable();

		door();
		door(bool open);
		door(crow::level* _lv, bool open = true);
		door(crow::level* _lv, bool _open, char _tile, crow::room* _roomptr, crow::door* _neighbor = nullptr);
		door(crow::level* _lv, bool _open, float2e _tile, crow::room* _roomptr, crow::door* _neighbor = nullptr);

		crow::door* neighbor = nullptr;
		// the door does not require a pointer to the panel in the current state as the panel has a pointer to the door
		//crow::door_panel* panel = nullptr;
	};

	struct door_panel : public interactible {
		virtual void interact(size_t const index, crow::entities& entity, int inter_index = -1);
		virtual void activate(crow::game_manager& state);
		door_panel(crow::level* _lv);
		door_panel(crow::level* _lv, char _tile, crow::door* _door, float2e offset = {0, 0});

		crow::door* door = nullptr;
	};

	struct sd_console : public interactible {
		// TODO: tick up optional task
		virtual void interact(size_t const index, crow::entities& entity, int inter_index = -1);
		virtual void activate(crow::game_manager& state);
		virtual void dissable();
		sd_console(crow::level* _lv);

		pg_console* power = nullptr;
	};

	struct exit : public interactible {
		// TODO: Make player transition
		virtual void interact(size_t const index, crow::entities& entity, int inter_index = -1);
		// third param of the constructor (level number) should ALWAYS be the level
		// number that the exit is IN, NOT the level number that it takes you to
		exit(game_manager* _state, crow::level* _lv, int _level_num);

		game_manager* state = nullptr;
		pg_console* power = nullptr;
		int level_num = 0;
	};

	// TODO: set up this interactible for the ai to kill the player
	struct player_interact : public interactible {
		// May not be needed, should be assigned to the player for the AI
		virtual void dissable();
		player_interact();
	};

	struct m_door_panel : public interactible {
		virtual void interact(size_t const index, crow::entities& entity, int inter_index = -1);
		virtual void activate(crow::game_manager& state);
		m_door_panel(crow::level* _lv);
		m_door_panel(crow::level* _lv, char _tile, float2e offset = {0, 0});

		std::vector<crow::door*> doors;
	};

}  // namespace crow