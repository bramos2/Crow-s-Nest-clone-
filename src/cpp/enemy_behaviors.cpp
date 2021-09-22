#include "../hpp/enemy_behaviors.hpp"
#include "../hpp/player_behavior.hpp"

void ai_manager::update_position(crow::entities objects,
                                 crow::entity enemy_index) {
  position = glm::vec2(objects.transforms_data[enemy_index][3][0],
                       objects.transforms_data[enemy_index][3][2]);
}

void ai_manager::update_target_position(crow::entities objects,
                                        crow::entity target_index) {
  current_target_position =
      glm::vec2(objects.transforms_data[target_index][3][0],
                objects.transforms_data[target_index][3][2]);
}

void ai_manager::set_current_room(std::shared_ptr<crow::map_room> room) {
  current_room = room;
}

void ai_manager::load_entity_data(crow::entities& objects,
                                  crow::entity enemy_index,
                                  crow::entity target_index) {
  velocity = &objects.velocities[enemy_index];
  update_position(objects, enemy_index);
  target_pos = glm::vec2{0, 0};
  update_target_position(objects, target_index);
}

void ai_manager::create_behavior_tree() {
  // child of my root node
  behavior_tree::sequence_node* seq1 = new behavior_tree::sequence_node();

  // 1st child of my sequence node
  behavior_tree::selector_node* sel1 = new behavior_tree::selector_node();

  // 1st child of selector 1
  has_target* ht1 = new has_target(adquired_target);

  // 2nd child of my selector 1
  get_target* gt1 = new get_target(adquired_target, &target_pos,
                                   current_target_position, path);

  sel1->add_child(ht1);
  sel1->add_child(gt1);

  seq1->add_child(sel1);

  // 2nd child of sequence node
  behavior_tree::selector_node* sel2 = new behavior_tree::selector_node();

  // 1st child of 2nd selector node
  has_path* hp1 = new has_path(path);

  // 2nd child of 2nd selector node
  ai_get_path* gp1 = new ai_get_path();
  gp1->adquired_target = &adquired_target;
  gp1->position = &position;
  gp1->target_position = &target_pos;
  gp1->current_room = current_room;
  gp1->path_container = &path;

  sel2->add_child(hp1);
  sel2->add_child(gp1);

  seq1->add_child(sel2);

  has_target_changed* htc1 = new has_target_changed();
  htc1->current_target_position = &current_target_position;
  htc1->target_check = &adquired_target;
  htc1->target_position = &target_pos;

  move_to_target* mtt1 = new move_to_target();
  mtt1->current_path = &path;
  mtt1->position = &position;
  mtt1->speed = 1.2f;
  mtt1->velocity = velocity;
  mtt1->adquired_target = &adquired_target;

  behavior_tree::sequence_node* seq2 = new behavior_tree::sequence_node();
  seq2->add_child(htc1);
  seq2->add_child(mtt1);

  seq1->add_child(seq2);

  b_tree.set_root_child(seq1);
}

void ai_manager::clean_tree() {
  // TODO: clean tree
}

status has_target::run() {
  if (*adquired_target) {
    // fmt::print("1: I have a target\n");
    return status::PASSED;
  }
  // fmt::print("1: I don't have a target\n");
  return status::FAILED;
}

has_target::has_target(bool& a_target) : adquired_target(&a_target) {}

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
  if (current_path->size() > 0) {
    // fmt::print("3: There is a path\n");
    return status::PASSED;
  }
  return status::FAILED;
}

has_path::has_path(std::vector<glm::vec2>& path) : current_path(&path) {}

status ai_get_path::run() {
  *path_container = current_room->get_path(*position, *target_position);
  if (path_container->size() == 0) {
    // fmt::print("4: failed to get path\n");
    *adquired_target = false;
    return status::FAILED;
  }
  // fmt::print("4: got a path to target\n");
  return status::PASSED;
}

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
  if (current_path->size() > 0) {
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
  *adquired_target = false;
  *velocity = glm::vec3{0, 0, 0};
  return status::PASSED;
}
