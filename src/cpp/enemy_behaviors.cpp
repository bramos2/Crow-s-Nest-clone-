#include "../hpp/enemy_behaviors.hpp"

#include "../hpp/player_behavior.hpp"

void ai_manager::update_position(crow::entities objects) {
  position = glm::vec2(
      objects.transforms_data[static_cast<size_t>(crow::entity::SPHYNX)][3][0],
      objects
          .transforms_data[static_cast<size_t>(crow::entity::SPHYNX)][3][2]);
}

void ai_manager::update_target_position(crow::entities objects,
                                        crow::entity target_index) {
  current_target_position =
      glm::vec2(objects.transforms_data[static_cast<size_t>(target_index)][3][0],
      objects.transforms_data[static_cast<size_t>(target_index)][3][2]);
}

void ai_manager::set_current_room(std::shared_ptr<crow::room> room) {
  current_room = room;
}

void ai_manager::load_entity_data(crow::entities& objects,
                                  crow::entity enemy_index,
                                  crow::entity target_index) {
  velocity = &objects.velocities[static_cast<size_t>(enemy_index)];
  update_position(objects);
  target_pos = glm::vec2{0, 0};
  update_target_position(objects, target_index);
}

void ai_manager::create_behavior_tree() {
  // child of my root node
  seq1 = new behavior_tree::sequence_node();

  // 1st child of my sequence node
  sel1 = new behavior_tree::selector_node();

  // 1st child of selector 1
  ht1 = new has_target(target);

  // 2nd child of my selector 1
  // gt1 = new get_target(adquired_target, &target_pos,
  //                                current_target_position, path);
  sc1 = new scan_room(current_room, target);

  sel1->add_child(ht1);
  sel1->add_child(sc1);

  seq1->add_child(sel1);

  // 2nd child of sequence node
  sel2 = new behavior_tree::selector_node();

  // 1st child of 2nd selector node
  hp1 = new has_path(path);

  // 2nd child of 2nd selector node
  gp1 = new ai_get_path();
  // gp1->adquired_target = &adquired_target;
  gp1->position = &position;
  // gp1->target_position = &target_pos;
  gp1->current_room = current_room;
  gp1->path_container = &path;
  gp1->target_i = target;

  sel2->add_child(hp1);
  sel2->add_child(gp1);

  seq1->add_child(sel2);

  // htc1 = new has_target_changed();
  // htc1->current_target_position = &current_target_position;
  // htc1->target_check = &adquired_target;
  // htc1->target_position = &target_pos;

  mtt1 = new move_to_target();
  mtt1->current_path = &path;
  mtt1->position = &position;
  mtt1->speed = 1.2f;
  mtt1->velocity = velocity;
  mtt1->target_i = target;
  // mtt1->adquired_target = &adquired_target;

  /*behavior_tree::sequence_node* seq2 = new behavior_tree::sequence_node();
  seq2->add_child(htc1);
  seq2->add_child(mtt1);*/

  seq1->add_child(mtt1);

  b_tree.set_root_child(seq1);
}

void ai_manager::clean_tree() {
  // TODO: clean tree
  delete seq1;
  delete sel1;
  delete ht1;
  delete sc1;
  delete sel2;
  delete hp1;
  delete gp1;
  delete mtt1;
}

status has_target::run() {
  if (target_i) {
    // fmt::print("1: I have a target\n");
    return status::PASSED;
  }
  // fmt::print("1: I don't have a target\n");
  return status::FAILED;
}

has_target::has_target(crow::interactible* a_target) : target_i(a_target) {}

status get_target::run() {
  // simply just updating the values for now, this node is temporary and will
  // be removed once the ai and the world becomes more complex
  // fmt::print("2: re-targeting, clearing path\n");
  *target = *current_target_pos;
  *adquired_target = true;
  current_path->clear();
  return status::PASSED;
}

get_target::get_target(bool& a_target, glm::vec2* t, glm::vec2& curr_t_pos,
                       std::vector<glm::vec2>& path)
    : adquired_target(&a_target),
      target(t),
      current_target_pos(&curr_t_pos),
      current_path(&path) {}

status has_path::run() {
  if (current_path->empty()) {
    // fmt::print("3: There is a path\n");
    return status::FAILED;
  }
  return status::PASSED;
}

has_path::has_path(std::vector<glm::vec2>& path) : current_path(&path) {}

status ai_get_path::run() {
  if (target_i) {
    glm::vec2 target_pos =
        current_room->get_tile_wpos(target_i->x, target_i->y);
    *path_container = current_room->get_path(*position, target_pos);
  }

  if (path_container->empty()) {
    // fmt::print("4: failed to get path\n");
    //*adquired_target = false;
    return status::FAILED;
  }
  // fmt::print("4: got a path to target\n");
  return status::PASSED;
}

// may not be needed for now
status has_target_changed::run() {
  if (*target_position == *current_target_position) {
    // fmt::print("5: target has not changed\n");
    return status::PASSED;
  }

  *target_check = false;
  // fmt::print("5: the target changed\n");
  return status::FAILED;
}

status move_to_target::run() {
  if (!current_path->empty()) {
    crow::set_velocity(*position, current_path->back(), *velocity, speed);
    if (velocity->x > 0 &&
        position->x + velocity->x >= current_path->back().x) {
      // fmt::print("6: removed node from path vector\n");
      current_path->pop_back();
    } else if (velocity->x < 0 &&
               position->x + velocity->x <= current_path->back().x) {
      // fmt::print("6: removed node from path vector\n");
      current_path->pop_back();
    }
    // fmt::print("6: move to target running\n");
    return status::RUNNING;
  }
  // fmt::print("6: reached target, stopping\n");
  //*adquired_target = false;
  target_i->dissable();
  target_i = nullptr;
  *velocity = glm::vec3{0, 0, 0};
  return status::PASSED;
}

// replacing get_target for now
status scan_room::run() {
  /*if (current_room->consoles.empty()) {
    return status::FAILED;
  }*/

  /*for (size_t i = 0; i < current_room->consoles.size(); i++) {
    if (!current_room->consoles[i]->is_broken) {
      target_i = current_room->consoles[i];
      break;
    }
  }*/

  return status::PASSED;
}

scan_room::scan_room(std::shared_ptr<crow::room> room,
                     crow::interactible*& target) {
  current_room = room;
  target = target_i;
}
