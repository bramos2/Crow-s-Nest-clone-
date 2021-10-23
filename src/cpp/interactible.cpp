#include "../hpp/interactible.hpp"

#include "../hpp/game_manager.hpp"
#include "../hpp/map.hpp"

namespace crow {

void interactible::interact(size_t const index, crow::entities& entity) {
  if (!is_broken) {
    printf("\n*****interacted with interactible*****\n");
  } else {
    printf("\n*****fixing interactible*****\n");
    is_broken = false;
  }
  is_active = !is_active;

  if (is_active) {
    printf("*****power generator online*****");
  } else {
    printf("*****power generator offline*****");
  }
}

void interactible::activate() { printf("something was activated.\n"); }

void interactible::dissable() {
  printf("\n*****interactible has been destroyed*****");
  is_broken = true;
  is_active = false;
}

void interactible::set_tile(unsigned int _x, unsigned int _y) {
  x = _x;
  y = _y;
}

interactible::interactible(unsigned int _x, unsigned int _y) : x(_x), y(_y) {}

void pg_console::interact(size_t const index, crow::entities& entity) {
  if (!is_broken) {
    printf("\n*****interacted with pg_console*****\n");
  } else {
    printf("\n*****fixing pg_console*****\n");
    is_broken = false;
  }
  is_active = true;
}

pg_console::pg_console() { type = crow::object_type::POWER_CONSOLE; }

oxygen_console::oxygen_console() {
  type = crow::object_type::OXYGEN_CONSOLE;
  is_active = false;
}

void oxygen_console::interact(size_t const index, crow::entities& entity) {
  is_active = true;
}

void door_panel::interact(size_t const index, crow::entities& entity) {
  std::string text;
  switch (panel_type) {
      // hackable door
    case 0:
      // if the door is unusable, make it useable. otherwise, toggle between
      // open and closed.
      switch (panel_status) {
        case 0:
          text = "ACTIVATING...";
          break;
        case 1:
          text = "LOCKING...";
          break;
        case 2:
          text = "UNLOCKING...";
          break;
      }
      break;
      // repairable door
    case 1:
      if (panel_status == 1) {
        current_level->msg = message("This door panel is already repaired!");
        return;
      } else
        text = "REPAIRING...";
      break;
  }
  current_level->interacting = this;
  current_level->msg = message(text, crow::default_message_time - 1.0f,
                               crow::default_interact_wait);
}

void door_panel::activate() {
  switch (panel_type) {
      // hackable door
    case 0:
      // if the door is unusable, make it useable. otherwise, toggle between
      // open and closed.
      switch (panel_status) {
        case 0:
          panel_status = 1;
          break;
        case 1:
          panel_status = 2;
          break;
        case 2:
          panel_status = 1;
          break;
      }
      break;
    case 1:
      // repairable doors can be repaired only. there is nothing to do to it
      // other than repair it. it shouldn't be able to be activated if it's
      // already repaired, but if it does get activated nothing will happen
      panel_status = 1;
      break;
  }
}

door_panel::door_panel(crow::level* _lv) {
  type = crow::object_type::DOOR_PANEL;
  current_level = _lv;
}

void sd_console::interact(size_t const index, crow::entities& entity) {
  if (!is_broken) {
    printf("\n*****interacted with sd_console*****\n");
  } else {
    printf("\n*****fixing sd_console*****\n");
    is_broken = false;
  }
  is_active = true;
}

sd_console::sd_console() { type = crow::object_type::SD_CONSOLE; }

void player_interact::dissable() {
  printf("\n*****enemy is attacking player\n");
  interactible::dissable();
}

player_interact::player_interact() {
  type = crow::object_type::PLAYER;
  is_active = true;
}

void door::interact(size_t const index, crow::entities& entity) {
  printf("\ninteracted with door, congrats!");

  // panel to look at
  // if this door doesn't have a panel attached, it will check to see if the
  // neighboring door has a panel
  crow::door_panel* _panel = panel;
  if (!_panel) _panel = neighbor->panel;

  // notifs are passed up via a "bubble up" method
  if (_panel != nullptr) {
    // door is unuseable, open up!
    if (_panel->panel_status == 0) {
      if (current_level) {
        // door can't open, give message to warn that the door is unusable
        current_level->msg = message(
            "The door is broken and needs to be repaired before it can be "
            "used.");
        if (_panel->panel_type == 0) {
          current_level->msg = message(
              "The door isn't working and needs to be reprogrammed before it "
              "can be used.");
        }
      } else {
        printf(
            "\nerror! tried to display a message, but the door doesn't have a "
            "reference to the current level!");
      }

      return;
      // door has been hacked shut by the player. give message to alert the
      // player of this.
    } else if (_panel->panel_status == 2) {
      if (current_level) {
        current_level->msg = message(
            "You've hacked this door shut. You need to unlock it before you "
            "can use it.");
      } else {
        printf(
            "\nerror! tried to display a message, but the door doesn't have a "
            "reference to the current level!");
      }
      return;
    }
  }

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
    float2e npos = neighbor->roomptr->get_tile_wpos(neighbor->x, neighbor->y);

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
door::door(crow::level* _lv) {
  type = crow::object_type::DOOR;
  current_level = _lv;
}

void exit::interact(size_t const index, crow::entities& entity) {
  printf("\ninteracted with exit, congrats! loaded level: %i",
             level_num + 1);
  state->change_level(level_num + 1);
}

exit::exit(game_manager* _state, crow::level* _lv, int _level_num) {
  type = crow::object_type::EXIT;
  current_level = _lv;
  state = _state;
  level_num = _level_num;
}

}  // namespace crow
