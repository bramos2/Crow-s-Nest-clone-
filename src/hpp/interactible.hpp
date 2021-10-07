#pragma once

#include "../hpp/game_objects.hpp"

namespace crow {

struct interactible {
  virtual void interact();
  virtual void dissable();
  void set_tile(unsigned int _x, unsigned int _y);
  // x and y coordinates in the tile map for the room that contains the item
  unsigned int x = 0;
  unsigned int y = 0;
  bool is_active = false;
  bool is_broken = false;
  crow::object_type type = crow::object_type::COUNT;

  interactible(unsigned int x, unsigned int y);
  interactible() = default;
};

struct pg_console : public interactible {
  // TODO: gives power to a level
  virtual void interact();
  pg_console();
};

struct door : public interactible {
  // TODO: Make player transition
  virtual void interact();
  door();
};

struct door_panel : public interactible {
  // TODO: hack door shut
  virtual void interact();
  door_panel();
};

struct sd_console : public interactible {
  // TODO: tick up optional task
  virtual void interact();
  sd_console();
};

struct player_interact : public interactible {
  // May not be needed, should be assigned to the player for the AI
  virtual void dissable();
  player_interact();
};

}  // namespace crow