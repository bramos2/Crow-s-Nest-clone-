#include "..\hpp\enemy_behaviors.hpp"

#include <time.h>

#include "../hpp/player_behavior.hpp"

namespace crow {

// void ai_manager::create_b_tree() {
//  //// allocating nodes for the tree
//  // nodes.resize(19);
//  //// 0 - 11 child nodes (12)
//  // for (size_t i = 0; i < 12; i++) {
//  //  nodes[i] = new behavior_tree::leaf_node();
//  //}
//
//  //// 12 - 14 sequence nodes (3)
//  // for (size_t i = 12; i < 15; i++) {
//  //  nodes[i] = new behavior_tree::sequence_node();
//  //}
//
//  //// 15 - 18 selector nodes (4)
//  // for (size_t i = 15; i < 19; i++) {
//  //  nodes[i] = new behavior_tree::selector_node();
//  //}
//
//  //// assigning behaviors to leaf nodes
//  // nodes[0]->r = &has_target;
//
//  //// connecting nodes to form tree
//}

// void ai_manager::clean_tree() {
//  /* for (auto& n : nodes) {
//     if (n) {
//       delete n;
//     }
//   }
//   nodes.clear();*/
//}

status roam_check(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  m.roam_timer += dt;
  if (m.roam_timer >= m.roam_total || m.curr_room->has_player) {
    m.is_roaming = false;
  }

  if (m.is_roaming) {
    result = crow::status::PASSED;
  }

  return result;
}

status roam_path(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;

  // we must get a ramdom path in this room if empty
  while (m.path.empty()){
    const unsigned int x = std::rand() % m.curr_room->width;
    const unsigned int y = std::rand() % m.curr_room->length;
    float2e goal = m.curr_room->get_tile_wpos(x, y);
    float3e temp_ai_pos = m.entities->get_world_position(
        static_cast<size_t>(crow::entity::SPHYNX));
    float2e start = float2e(temp_ai_pos.x, temp_ai_pos.z);

    m.path = m.curr_room->get_path(start, goal);
  }
  // must always pass in this instance
  result = crow::status::PASSED;
  return result;
}

// target branch
status has_target(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  if (m.target && !m.curr_room->has_player) {
    if (m.target->roomptr == m.curr_room) {
      result = crow::status::PASSED;
    } else {
      m.target = nullptr;
      m.interacting = false;
    }
  } else if (m.target && m.target->type == crow::object_type::PLAYER &&
             m.curr_room->has_player) {
    result = crow::status::PASSED;
  }

  return result;
}

status target_player(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  // we need to check if the player is in this room
  if (!m.curr_room) {
    return result;
  }

  // will require that the room with the player has an interactible for the
  // player in it at all times
  // for (auto& obj : m.curr_room->objects) {
  //  if (obj->type == crow::object_type::PLAYER) {
  //    m.target = obj;
  //   // m.interacting = true;
  //    result = crow::status::PASSED;
  //    break;
  //  }
  //}

  if (m.curr_room->has_player) {
    m.target = m.curr_level->p_inter;
    result = crow::status::PASSED;
  }

  return result;
}

status target_console(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;

  if (m.counter == 0) {
    for (auto& obj : m.curr_room->objects) {
      // will need to check for the different consoles going forward
      if (obj->type == crow::object_type::POWER_CONSOLE) {
        m.target = obj;
        // m.interacting = true;
        result = crow::status::PASSED;
        break;
      }
    }
  }

  return result;
}

// TODO: CHANGE DOOR TARGETING
status target_door(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  // we will store a pointer to all doors in this room
  std::vector<crow::interactible*> room_doors;

  for (auto& obj : m.curr_room->objects) {
    if (obj->type == crow::object_type::DOOR) {
      room_doors.push_back(obj);
    }
  }

  if (room_doors.empty()) {
    return result;
  }

  //crow::interactible* selected_door = room_doors.back();

  // we will eliminate any previously used doors to avoid just going back and
  // forth, currently won't work as pre_t is in another room
  // if (room_doors.size() > 1) {
  //  for (auto& d : room_doors) {
  //    if (d == m.prev_target) {
  //      d = nullptr;
  //    }
  //  }
  //} else {
  //  selected_door = room_doors.back();
  //}

  // std::srand(std::time(0));

  //// time to select a new door
  // while (!selected_door) {
  //  size_t i = static_cast<size_t>((rand() % room_doors.size()));
  //  selected_door = room_doors[i];
  //}

  std::vector<size_t> tinx;
  tinx.push_back(0);

  for (size_t i = 0; i < room_doors.size(); i++) {
    if (i != tinx.back()) {
      if (room_doors[i]->heat > room_doors[tinx.back()]->heat) {
        tinx.clear();
        tinx.push_back(i);
      } else if (room_doors[i]->heat == room_doors[tinx.back()]->heat) {
        tinx.push_back(i);
      }
    }
  }

  size_t choice = rand() % tinx.size();

 // selected_door = 

  result = crow::status::PASSED;
  m.target = room_doors[tinx[choice]];
  //  m.interacting = true;

  // we should probably do this after interacting with a target
  // m.prev_target = m.target;

  return result;
}

status target_floor(float dt, crow::ai_manager& m) { return status(); }

// path branch
status has_path(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;

  if (!m.path.empty() || (m.path.empty() && m.interacting)) {
    result = crow::status::PASSED;
  }

  return result;
}

status is_path_currernt(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;

  // we need to check if this path is set to the current target, otherwise we
  // must clean it and get a new one
  // non moving target, use its tile
  if (m.path.empty() && m.interacting) {
    result = crow::status::PASSED;
  } else {
    if (m.target->type != crow::object_type::PLAYER) {
      float2e temp = m.curr_room->get_tile_wpos(m.target->x, m.target->y);

      if (temp == m.path[0]) {
        result = crow::status::PASSED;
      }
    } else {  // for player use entities
      float3e temp = m.entities->get_world_position(
          static_cast<size_t>(crow::entity::WORKER));
      float2e t = {temp.x, temp.z};

      float2e diff = m.path[0] - t;
      // if not too far appart the path is fine
      if (std::fabsf(diff.x) <= 1.f && std::fabsf(diff.y) <= 1.f) {
        result = crow::status::PASSED;
      }
    }
  }

  return result;
}

status get_path(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;

  m.path.clear();
  float3e temp_ai_pos =
      m.entities->get_world_position(static_cast<size_t>(crow::entity::SPHYNX));

  float2e target_pos;
  if (m.target->type == crow::object_type::PLAYER) {
    float3e p_pos = m.entities->get_world_position(
        static_cast<size_t>(crow::entity::WORKER));
    target_pos = {p_pos.x, p_pos.z};
  } else {
    target_pos = m.curr_room->get_tile_wpos(m.target->x, m.target->y);
  }

  m.path = m.curr_room->get_path(float2e(temp_ai_pos.x, temp_ai_pos.y),
                                 target_pos);
  m.interacting = true;
  crow::status::PASSED;

  return result;
}

// move branch
status reached_target(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;

  /* size_t index = static_cast<size_t>(crow::entity::SPHYNX);
   float3e curr_pos = m.entities->get_world_position(index);
   float2e target_pos;
   if (m.target->type == crow::object_type::PLAYER) {
     float3e p_pos = m.entities->get_world_position(
         static_cast<size_t>(crow::entity::WORKER));
     target_pos = {p_pos.x, p_pos.z};
   } else {
     target_pos = m.curr_room->get_tile_wpos(m.target->x, m.target->y);
   }*/

  // || crow::reached_destination({0.f, 0.f}, {curr_pos.x, curr_pos.z},
  // target_pos)
  if (m.path.empty()) {
      size_t index = static_cast<size_t>(crow::entity::SPHYNX);
    result = crow::status::PASSED;
    m.entities->velocities[index] = {
        0.f, 0.f, 0.f};
    if (!m.entities->mesh_ptrs[index]->animator->is_acting && !m.entities->mesh_ptrs[index]->animator->performed_action) {
        m.entities->mesh_ptrs[index]->animator->switch_animation(1);
        m.entities->mesh_ptrs[index]->animator->performed_action = true;
    }
  }

  return result;
}

status move(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  size_t index = static_cast<size_t>(crow::entity::SPHYNX);
  crow::set_velocity(m.path.back(), *m.entities, index, m.roam_speed);
  float3e curr_pos = m.entities->get_world_position(index);

  float2e curr_vel = float2e(dt * m.entities->velocities[index].x,
                                 dt * m.entities->velocities[index].z);

  if (crow::reached_destination(curr_vel, float2e(curr_pos.x, curr_pos.z),
                                m.path.back())) {
    m.path.pop_back();
  }

  result = crow::status::RUNNING;

  return result;
}

status check_room(float dt, crow::ai_manager& m) { return status(); }

// interaction branch
status is_target_door(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;

  if (m.target->type == crow::object_type::DOOR) {
    result = crow::status::PASSED;
  }

  return result;
}

// TODO: Improve door handling
status handle_door(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  size_t index = static_cast<size_t>(crow::entity::SPHYNX);
  if (!m.entities->mesh_ptrs[index]->animator->is_acting) {
      // door is closed
      if (m.target->is_active == false) {
          // TODO: check logic for closed door as it always seems to destroy the door
          // instead of going for another target
          unsigned int d_count = 0;
          std::vector<crow::interactible*> doors;
          for (auto& d : m.curr_room->objects) {
              if (d->type == crow::object_type::DOOR) {
                  ++d_count;
                  doors.push_back(d);
              }
          }

          if (d_count > 1) {
              unsigned roll = static_cast<unsigned int>((rand() % 2));
              if (roll == 0) {
                  m.target->dissable();
                  m.target->interact(index, *m.entities);

              }
              else {
                  crow::interactible* selected_door = nullptr;

                  while (!selected_door) {
                      unsigned int i = static_cast<unsigned int>((rand() % doors.size()));
                      selected_door = doors[i];
                  }
                  m.prev_target = m.target;
                  m.target = selected_door;
              }

          }
          else {
              m.target->dissable();
              m.target->interact(index, *m.entities);
              m.prev_target = m.target;
              m.target = nullptr;
          }

      }
      else {
          m.target->interact(index, *m.entities);
          m.prev_target = m.target;
          m.target = nullptr;
      }

      m.room_check();
      m.interacting = false;
      m.is_roaming = true;
      m.roam_timer = 0.f;
      m.entities->mesh_ptrs[index]->animator->performed_action = false;
      result = crow::status::PASSED;
  }
  else {
      result = crow::status::RUNNING;
  }

  return result;
}

status destroy_target(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  size_t index = static_cast<size_t>(crow::entity::SPHYNX);

  if (m.entities->mesh_ptrs[index]->animator && !m.entities->mesh_ptrs[index]->animator->is_acting) {

      m.target->dissable();
      m.prev_target = m.target;
      m.target = nullptr;
      m.counter--;
      m.interacting = false;
      m.entities->mesh_ptrs[index]->animator->performed_action = false;
      result = crow::status::PASSED;
  }
  else {
      result = crow::status::RUNNING;
  }

  return result;
}

void ai_manager::room_check() {
  bool finished = false;
  for (auto& i : curr_level->rooms) {
    for (auto& j : i) {
      if (j.has_ai) {
        finished = true;
        curr_room = &j;
        break;
      }
    }
    if (finished) {
      break;
    }
  }
}

void ai_manager::init_manager(crow::entities* ent, crow::level* level) {
  entities = ent;
  curr_level = level;
  this->room_check();
}

}  // namespace crow