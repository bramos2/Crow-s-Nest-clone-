#pragma once

struct interactible {
  virtual void interact() = 0;
  // y coordinate in the tile map for the room that contains the item
  unsigned int x = 0;
  // x coordinate in the tile map for the room that contains the item
  unsigned int y = 0;
  // index of item in entity system
  unsigned int index = 0;
  unsigned int mesh_inx = 0;
};

struct sd_console : public interactible {
  // TODO: tick win condition
  virtual void interact() override;
};

struct pg_console : public interactible {
  // TODO: POWER A BLOCK
  virtual void interact() override;
};

enum class item_type { NONE = -1, BEAKER = 0, FLASK, ALARM, RADIO, BAR };

struct item : public interactible {
  // TODO: Pickup item, place on player inv, switch with player inv item
  virtual void interact() override;
  // TODO: DEFINE BEHAVIORS OF EACH ITEM
  void use_item();

  item* player_inv = nullptr;
  item_type type = item_type::NONE;
};