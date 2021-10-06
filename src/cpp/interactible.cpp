#include "../hpp/interactible.hpp"

#include <liblava-extras/fbx.hpp>

// void sd_console::interact() {
//  *win_condition += 1;
//  active = true;
//  // TODO: REMOVE
//  fmt::print("\ninteracted with self_destruct console\n");
//}

void pg_console::interact() {
  if (!is_broken) {
    fmt::print("\n*****interacted with pg_console*****\n");
  } else {
    fmt::print("\n*****fixing pg_console*****\n");
    is_broken = false;
  }
  is_active = true;
}

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

void door_panel::interact() {}

void sd_console::interact() {
  if (!is_broken) {
    fmt::print("\n*****interacted with sd_console*****\n");
  } else {
    fmt::print("\n*****fixing sd_console*****\n");
    is_broken = false;
  }
  is_active = true;
}

void player_interact::dissable() {
  fmt::print("\n*****enemy is attacking player\n");
  interactible::dissable();
}

player_interact::player_interact() { is_active = true; }
