#pragma once
#include "../hpp/interactible.hpp"

// this header will deal with the inventory and the logic of throwing objects

struct player_inventory {
  crow::item current_item;

  player_inventory() {
    current_item.item_id = crow::item_type::NONE;
    current_item.index = -1;
    //current_item.contained_room = nullptr;
  }
};