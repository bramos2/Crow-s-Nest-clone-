#pragma once

#include "../hpp/entities.hpp"
#include "../hpp/game_objects.hpp"

namespace crow {

struct room;

struct interactible {
  virtual void interact(size_t const index, crow::entities& entity);
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

  crow::door* neighbor = nullptr;
};

struct door_panel : public interactible {
  // TODO: hack door shut
  virtual void interact(size_t const index, crow::entities& entity);
  door_panel();

  crow::door* door = nullptr;
};

struct sd_console : public interactible {
  // TODO: tick up optional task
  virtual void interact(size_t const index, crow::entities& entity);
  sd_console();
};

// TODO: set up this interactible for the ai to kill the player
struct player_interact : public interactible {
  // May not be needed, should be assigned to the player for the AI
  virtual void dissable();
  player_interact();
};

}  // namespace crow