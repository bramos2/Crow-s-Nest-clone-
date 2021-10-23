#pragma once

#include <iostream>

#include "../hpp/entities.hpp"
#include "../hpp/message.hpp"

namespace crow {
	// forward declares for circular includes
	class game_manager;
	struct room;
	struct level;
	struct door_panel;

	enum class object_type {
	  PLAYER = 0,
	  ENEMY = 1,
	  SD_CONSOLE,
	  POWER_CONSOLE,
	  DOOR,
	  EXIT,
	  DOOR_PANEL,
	  OXYGEN_CONSOLE,
	  PRESSURE_CONSOLE,
	  FLOOR,
	  WALL,
	  SHORT_WALL,
	  COUNT,
	};

	// when current_level == final_level, we will skip to the win screen instead of
	// attempting to advance to the next level
	static constexpr int final_level = 10;


	struct interactible {
	  virtual void interact(size_t const index, crow::entities& entity);
	  // similar to interact(), but for objects that require time to activate
	  virtual void activate();
	  virtual void dissable();
	  void set_tile(unsigned int _x, unsigned int _y);
	  // x and y coordinates in the tile map for the room that contains the item
	  unsigned int x = 0;
	  unsigned int y = 0;
	  bool is_active = false;
	  bool is_broken = false;
	  int heat = 0;
	  crow::object_type type = crow::object_type::COUNT;
	  crow::room* roomptr = nullptr;
	  crow::level* current_level = nullptr;
	  interactible(unsigned int x, unsigned int y);
	  interactible() = default;
	};

	struct pg_console : public interactible {
	  // TODO: gives power to a level
	  virtual void interact(size_t const index, crow::entities& entity);
	  pg_console();
	};

	struct oxygen_console : public interactible {
	  // TODO: gives power to a level
	  virtual void interact(size_t const index, crow::entities& entity);
	  oxygen_console();
	};

	struct door : public interactible {
	  // TODO: Make player transition
	  virtual void interact(size_t const index, crow::entities& entity);
	  door();
	  door(crow::level* _lv);

	  crow::door* neighbor = nullptr;
	  crow::door_panel* panel = nullptr;
	};

	struct door_panel : public interactible {
	  virtual void interact(size_t const index, crow::entities& entity);
	  virtual void activate();
	  door_panel(crow::level* _lv);

	  crow::door* door = nullptr;
	  // 0 = hackable door
	  // 1 = repairable door
	  char panel_type = 0;
	  // for hackable doors:
	  //	0 = door can't be used (need to hack it first)
	  //	1 = door is open and can be closed
	  //	2 = door is closed and can be open
	  // for repairable doors:
	  //	0 = door is broken (needs to be repaired first)
	  //	1 = door is fixed (no need to repair it)
	  char panel_status = 0;
	};

	struct sd_console : public interactible {
	  // TODO: tick up optional task
	  virtual void interact(size_t const index, crow::entities& entity);
	  sd_console();
	};

	struct exit : public interactible {
	  // TODO: Make player transition
	  virtual void interact(size_t const index, crow::entities& entity);
	  // third param of the constructor (level number) should ALWAYS be the level
	  // number that the exit is IN, NOT the level number that it takes you to
	  exit(game_manager* _state, crow::level* _lv, int _level_num);

	  game_manager* state = nullptr;
	  int level_num = 0;
	};

	// TODO: set up this interactible for the ai to kill the player
	struct player_interact : public interactible {
	  // May not be needed, should be assigned to the player for the AI
	  virtual void dissable();
	  player_interact();
	};

}  // namespace crow