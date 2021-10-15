#include "../hpp/interactible.hpp"

#include <liblava-extras/fbx.hpp>

#include "../hpp/map.hpp"

namespace crow {

void pg_console::interact(size_t const index, crow::entities& entity) {
  if (!is_broken) {
    fmt::print("\n*****interacted with pg_console*****\n");
  } else {
    fmt::print("\n*****fixing pg_console*****\n");
    is_broken = false;
  }
  is_active = true;
}

pg_console::pg_console() { type = crow::object_type::POWER_CONSOLE; }

void interactible::interact(size_t const index, crow::entities& entity) {
  if (!is_broken) {
    fmt::print("\n*****interacted with interactible*****\n");
  } else {
    fmt::print("\n*****fixing interactible*****\n");
    is_broken = false;
  }
  is_active = !is_active;

  if (is_active) {
    fmt::print("*****power generator online*****");
  } else {
    fmt::print("*****power generator offline*****");
  }
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

void door_panel::interact(size_t const index, crow::entities& entity) {}

door_panel::door_panel() { type = crow::object_type::DOOR_PANEL; }

void sd_console::interact(size_t const index, crow::entities& entity) {
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

player_interact::player_interact() {
  type = crow::object_type::PLAYER;
  is_active = true;
}

void door::interact(size_t const index, crow::entities& entity) {
  fmt::print("\ninteracted with door, congrats!");

  // we will be moving the index out of this room
  if (!roomptr || !neighbor) {
    return;
  }

  if (!roomptr->object_indices.empty()) {
    if (roomptr->object_indices.back() != index) {
      const size_t temp = roomptr->object_indices.back();

      for (auto& i : roomptr->object_indices) {
        if (i == index) {
          i = temp;
          break;
        }
      }

      roomptr->object_indices.back() = index;
    }

    roomptr->object_indices.pop_back();

    neighbor->roomptr->object_indices.push_back(index);
    glm::vec2 npos = neighbor->roomptr->get_tile_wpos(neighbor->x, neighbor->y);

    entity.set_world_position(index, npos.x, 0.f, npos.y);
    if (index == static_cast<size_t>(crow::entity::WORKER)) {
      roomptr->has_player = false;
      neighbor->roomptr->has_player = true;
      heat++;
      neighbor->heat++;
    } else {
      roomptr->has_ai = false;
      neighbor->roomptr->has_ai = true;
      heat--;
      neighbor->heat--;
    }
  }


}

door::door() {
  type = crow::object_type::DOOR;
  is_active = true;
  is_broken = false;
}

}  // namespace crow
