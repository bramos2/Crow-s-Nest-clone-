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

// target branch
status has_target(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  if (m.target && !m.curr_room->has_player) {
    if (m.curr_room && m.target->roomptr == m.curr_room) {
      result = crow::status::PASSED;
    } else {
      m.target = nullptr;
      m.interacting = false;
    }
  } else if (m.target && m.target->type == crow::object_type::PLAYER && m.curr_room->has_player) {
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

  crow::interactible* selected_door = nullptr;

  // we will eliminate any previously used doors to avoid just going back and
  // forth, currently won't work as pre_t is in another room
  if (room_doors.size() > 1) {
    for (auto& d : room_doors) {
      if (d == m.prev_target) {
        d = nullptr;
      }
    }
  } else {
    selected_door = room_doors.back();
  }

  std::srand(std::time(0));

  // time to select a new door
  while (!selected_door) {
    size_t i = static_cast<size_t>((rand() % room_doors.size()));
    selected_door = room_doors[i];
  }

  result = crow::status::PASSED;
  m.target = selected_door;
  //  m.interacting = true;

  // we should probably do this after interacting with a target
  //m.prev_target = m.target;

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
      glm::vec2 temp = m.curr_room->get_tile_wpos(m.target->x, m.target->y);

      if (temp == m.path[0]) {
        result = crow::status::PASSED;
      }
    } else {  // for player use entities
      glm::vec3 temp = m.entities->get_world_position(
          static_cast<size_t>(crow::entity::WORKER));
      glm::vec2 t = {temp.x, temp.z};

      glm::vec2 diff = m.path[0] - t;
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
  glm::vec3 temp_ai_pos =
      m.entities->get_world_position(static_cast<size_t>(crow::entity::SPHYNX));

  glm::vec2 target_pos;
  if (m.target->type == crow::object_type::PLAYER) {
    glm::vec3 p_pos = m.entities->get_world_position(
        static_cast<size_t>(crow::entity::WORKER));
    target_pos = {p_pos.x, p_pos.z};
  } else {
    target_pos = m.curr_room->get_tile_wpos(m.target->x, m.target->y);
  }

  m.path = m.curr_room->get_path(glm::vec2(temp_ai_pos.x, temp_ai_pos.y),
                                 target_pos);
  m.interacting = true;
  crow::status::PASSED;

  return result;
}

// move branch
status reached_target(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;

 /* size_t index = static_cast<size_t>(crow::entity::SPHYNX);
  glm::vec3 curr_pos = m.entities->get_world_position(index);
  glm::vec2 target_pos;
  if (m.target->type == crow::object_type::PLAYER) {
    glm::vec3 p_pos = m.entities->get_world_position(
        static_cast<size_t>(crow::entity::WORKER));
    target_pos = {p_pos.x, p_pos.z};
  } else {
    target_pos = m.curr_room->get_tile_wpos(m.target->x, m.target->y);
  }*/

  // || crow::reached_destination({0.f, 0.f}, {curr_pos.x, curr_pos.z}, target_pos)
  if (m.path.empty() ) {
    result = crow::status::PASSED;
    m.entities->velocities[static_cast<size_t>(crow::entity::SPHYNX)] = {
        0.f, 0.f, 0.f};
  }

  return result;
}

status move(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  size_t index = static_cast<size_t>(crow::entity::SPHYNX);
  crow::set_velocity(m.path.back(), *m.entities, index, m.roam_speed);
  glm::vec3 curr_pos = m.entities->get_world_position(index);

  glm::vec2 curr_vel = glm::vec2(dt * m.entities->velocities[index].x,
                                 dt * m.entities->velocities[index].z);

  if (crow::reached_destination(curr_vel, glm::vec2(curr_pos.x, curr_pos.z),
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

status handle_door(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  size_t index = static_cast<size_t>(crow::entity::SPHYNX);

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

      } else {
        crow::interactible* selected_door = nullptr;

        while (!selected_door) {
          unsigned int i = static_cast<unsigned int>((rand() % doors.size()));
          selected_door = doors[i];
        }
        m.prev_target = m.target;
        m.target = selected_door;
      }

    } else {
      m.target->dissable();
      m.target->interact(index, *m.entities);
      m.prev_target = m.target;
      m.target = nullptr;
    }

  } else {
    m.target->interact(index, *m.entities);
    m.prev_target = m.target;
    m.target = nullptr;
  }

  m.room_check();
  m.interacting = false;
  result = crow::status::PASSED;

  return result;
}

status destroy_target(float dt, crow::ai_manager& m) {
  status result = crow::status::FAILED;
  size_t index = static_cast<size_t>(crow::entity::SPHYNX);

  m.target->dissable();
  m.prev_target = m.target;
  m.target = nullptr;
  m.counter--;
  m.interacting = false;
  result = crow::status::PASSED;

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

//#include "../hpp/enemy_behaviors.hpp"
//
//#include "../hpp/player_behavior.hpp"
//
// void ai_manager::update_position(crow::entities objects) {
//  position = glm::vec2(
//      objects.transforms_data[static_cast<size_t>(crow::entity::SPHYNX)][3][0],
//      objects.transforms_data[static_cast<size_t>(crow::entity::SPHYNX)][3][2]);
//}
//
// void ai_manager::update_target_position(crow::entities objects,
//                                        crow::entity target_index) {
//  current_target_position = glm::vec2(
//      objects.transforms_data[static_cast<size_t>(target_index)][3][0],
//      objects.transforms_data[static_cast<size_t>(target_index)][3][2]);
//}
//
// void ai_manager::set_current_room(std::shared_ptr<crow::room> room) {
//  current_room = room;
//}
//
// void ai_manager::load_entity_data(crow::entities& objects,
//                                  crow::entity enemy_index,
//                                  crow::entity target_index) {
//  velocity = &objects.velocities[static_cast<size_t>(enemy_index)];
//  update_position(objects);
//  target_pos = glm::vec2{0, 0};
//  update_target_position(objects, target_index);
//}
//
// void ai_manager::create_behavior_tree() {
//  // child of my root node
//  seq1 = new behavior_tree::sequence_node();
//
//  // 1st child of my sequence node
//  sel1 = new behavior_tree::selector_node();
//
//  // 1st child of selector 1
//  ht1 = new has_target(target);
//
//  // 2nd child of my selector 1
//  // gt1 = new get_target(adquired_target, &target_pos,
//  //                                current_target_position, path);
//  sc1 = new scan_room(current_room, target);
//
//  sel1->add_child(ht1);
//  sel1->add_child(sc1);
//
//  seq1->add_child(sel1);
//
//  // 2nd child of sequence node
//  sel2 = new behavior_tree::selector_node();
//
//  // 1st child of 2nd selector node
//  hp1 = new has_path(path);
//
//  // 2nd child of 2nd selector node
//  gp1 = new ai_get_path();
//  // gp1->adquired_target = &adquired_target;
//  gp1->position = &position;
//  // gp1->target_position = &target_pos;
//  gp1->current_room = current_room;
//  gp1->path_container = &path;
//  gp1->target_i = target;
//
//  sel2->add_child(hp1);
//  sel2->add_child(gp1);
//
//  seq1->add_child(sel2);
//
//  // htc1 = new has_target_changed();
//  // htc1->current_target_position = &current_target_position;
//  // htc1->target_check = &adquired_target;
//  // htc1->target_position = &target_pos;
//
//  mtt1 = new move_to_target();
//  mtt1->current_path = &path;
//  mtt1->position = &position;
//  mtt1->speed = 1.2f;
//  mtt1->velocity = velocity;
//  mtt1->target_i = target;
//  // mtt1->adquired_target = &adquired_target;
//
//  /*behavior_tree::sequence_node* seq2 = new behavior_tree::sequence_node();
//  seq2->add_child(htc1);
//  seq2->add_child(mtt1);*/
//
//  seq1->add_child(mtt1);
//
//  b_tree.set_root_child(seq1);
//}
//
// void ai_manager::clean_tree() {
//  // TODO: clean tree
//  delete seq1;
//  delete sel1;
//  delete ht1;
//  delete sc1;
//  delete sel2;
//  delete hp1;
//  delete gp1;
//  delete mtt1;
//}
//
// status has_target::run() {
//  if (target_i) {
//    // fmt::print("1: I have a target\n");
//    return status::PASSED;
//  }
//  // fmt::print("1: I don't have a target\n");
//  return status::FAILED;
//}
//
// has_target::has_target(crow::interactible* a_target) : target_i(a_target) {}
//
// status get_target::run() {
//  // simply just updating the values for now, this node is temporary and will
//  // be removed once the ai and the world becomes more complex
//  // fmt::print("2: re-targeting, clearing path\n");
//  *target = *current_target_pos;
//  *adquired_target = true;
//  current_path->clear();
//  return status::PASSED;
//}
//
// get_target::get_target(bool& a_target, glm::vec2* t, glm::vec2& curr_t_pos,
//                       std::vector<glm::vec2>& path)
//    : adquired_target(&a_target),
//      target(t),
//      current_target_pos(&curr_t_pos),
//      current_path(&path) {}
//
// status has_path::run() {
//  if (current_path->empty()) {
//    // fmt::print("3: There is a path\n");
//    return status::FAILED;
//  }
//  return status::PASSED;
//}
//
// has_path::has_path(std::vector<glm::vec2>& path) : current_path(&path) {}
//
// status ai_get_path::run() {
//  if (target_i) {
//    glm::vec2 target_pos =
//        current_room->get_tile_wpos(target_i->x, target_i->y);
//    *path_container = current_room->get_path(*position, target_pos);
//  }
//
//  if (path_container->empty()) {
//    // fmt::print("4: failed to get path\n");
//    //*adquired_target = false;
//    return status::FAILED;
//  }
//  // fmt::print("4: got a path to target\n");
//  return status::PASSED;
//}
//
//// may not be needed for now
// status has_target_changed::run() {
//  if (*target_position == *current_target_position) {
//    // fmt::print("5: target has not changed\n");
//    return status::PASSED;
//  }
//
//  *target_check = false;
//  // fmt::print("5: the target changed\n");
//  return status::FAILED;
//}
//
// status move_to_target::run() {
//  if (!current_path->empty()) {
//    crow::set_velocity(*position, current_path->back(), *velocity, speed);
//    if (velocity->x > 0 &&
//        position->x + velocity->x >= current_path->back().x) {
//      // fmt::print("6: removed node from path vector\n");
//      current_path->pop_back();
//    } else if (velocity->x < 0 &&
//               position->x + velocity->x <= current_path->back().x) {
//      // fmt::print("6: removed node from path vector\n");
//      current_path->pop_back();
//    }
//    // fmt::print("6: move to target running\n");
//    return status::RUNNING;
//  }
//  // fmt::print("6: reached target, stopping\n");
//  //*adquired_target = false;
//  target_i->dissable();
//  target_i = nullptr;
//  *velocity = glm::vec3{0, 0, 0};
//  return status::PASSED;
//}
//
//// replacing get_target for now
// status scan_room::run() {
//  /*if (current_room->consoles.empty()) {
//    return status::FAILED;
//  }*/
//
//  /*for (size_t i = 0; i < current_room->consoles.size(); i++) {
//    if (!current_room->consoles[i]->is_broken) {
//      target_i = current_room->consoles[i];
//      break;
//    }
//  }*/
//
//  return status::PASSED;
//}
//
// scan_room::scan_room(std::shared_ptr<crow::room> room,
//                     crow::interactible*& target) {
//  current_room = room;
//  target = target_i;
//}
