#pragma once

#include <liblava/lava.hpp>

#include "../hpp/entities.hpp"
#include "../hpp/game_objects.hpp"
#include "../hpp/message.hpp"

namespace crow {

struct room;
struct level;
struct door_panel;

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
  exit(lava::app* _app, crow::level* _lv, int _level_num);

  lava::app* app = nullptr;
  int level_num = 0;
};

// TODO: set up this interactible for the ai to kill the player
struct player_interact : public interactible {
  // May not be needed, should be assigned to the player for the AI
  virtual void dissable();
  player_interact();
};

}  // namespace crow