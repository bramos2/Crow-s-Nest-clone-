#include "../hpp/interactible.hpp"

#include <liblava-extras/fbx.hpp>

namespace crow {

void pg_console::interact() {
  if (!is_broken) {
    fmt::print("\n*****interacted with pg_console*****\n");
  } else {
    fmt::print("\n*****fixing pg_console*****\n");
    is_broken = false;
  }
  is_active = true;
}

pg_console::pg_console() { type = crow::object_type::POWER_CONSOLE; }

void interactible::interact() {
  if (!is_broken) {
    fmt::print("\n*****interacted with interactible*****\n");
  } else {
    fmt::print("\n*****fixing interactible*****\n");
    is_broken = false;
  }
  is_active = true;
}

void interactible::dissable() {
  fmt::print("\n*****interactible has been destroyed*****");
  is_broken = true;
  is_active = false;
}

void interactible::set_tile(unsigned int _x, unsigned int _y) {
  x = _x;
  y = _y;
}

interactible::interactible(unsigned int _x, unsigned int _y) : x(_x), y(_y) {}

void door_panel::interact() {}

door_panel::door_panel() { type = crow::object_type::DOOR_PANEL; }

void sd_console::interact() {
  if (!is_broken) {
    fmt::print("\n*****interacted with sd_console*****\n");
  } else {
    fmt::print("\n*****fixing sd_console*****\n");
    is_broken = false;
  }
  is_active = true;
}

sd_console::sd_console() { type = crow::object_type::SD_CONSOLE; }

void player_interact::dissable() {
  fmt::print("\n*****enemy is attacking player\n");
  interactible::dissable();
}

player_interact::player_interact() { is_active = true; }

void door::interact() {}

door::door() { type = crow::object_type::DOOR; }

}  // namespace crow
