#include "../hpp/interactible.hpp"

#include <liblava-extras/fbx.hpp>

#include <iostream>

void sd_console::interact() {
  *win_condition += 1;
  active = true;
  // TODO: REMOVE
  fmt::print("\ninteracted with self_destruct console\n");
}

void pg_console::interact() {}

void item::interact() {
  // switching the types of the player with this item
  if (player_inv) {
    const item_type temp_type = player_inv->type;
    const unsigned int temp_mi = player_inv->mesh_inx;

    player_inv->type = this->type;
    player_inv->mesh_inx = this->mesh_inx;
    this->type = temp_type;
    this->mesh_inx = temp_mi;
  }
}

void item::use_item() {
  // here we'll check the item type to execute a behavior
  switch (this->type) {
    case item_type::NONE: {
      break;
    }
    case item_type::BEAKER: {
      break;
    }
    case item_type::FLASK: {
      break;
    }
    case item_type::ALARM: {
      break;
    }
    case item_type::RADIO: {
      break;
    }
    case item_type::BAR: {
      break;
    }
    default:
      break;
  }
}
