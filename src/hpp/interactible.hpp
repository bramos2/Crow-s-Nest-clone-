#pragma once

struct interactible {
  virtual void interact();
  virtual void dissable();
  void set_tile(unsigned int _x, unsigned int _y);
  // y coordinate in the tile map for the room that contains the item
  unsigned int x = 0;
  // x coordinate in the tile map for the room that contains the item
  unsigned int y = 0;
  // index of item in entity system
  unsigned int index = 0;
  bool is_active = false;
  bool is_broken = false;
};

struct pg_console : public interactible {
  // TODO: gives power to a level
  virtual void interact();
};

struct door_panel : public interactible {
  // TODO: hack door shut
  virtual void interact();
};

struct sd_console : public interactible {
 public:
  virtual void interact();
};

struct player_interact : public interactible {
  virtual void dissable();

  player_interact();
};
