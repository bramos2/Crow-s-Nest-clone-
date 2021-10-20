#pragma once

#include "../hpp/behavior_tree.hpp"
#include "../hpp/enemy_behaviors.hpp"


// this class is meant to keep track of the ai by holding references to data the
// behavior tree will use
class ai_manager {
 public:
  // check to know if we currently have a target
  bool adquired_target = false;
  // the velocity of the ai entity
  glm::vec3* velocity;
  // the position of the ai entity
  glm::vec2 position;
  // the target position currently being used for path finding, could be a door,
  // the player or a noise signal
  glm::vec2 target_pos;
  // used to consider if the ai needs to do a new path find
  glm::vec2 current_target_position;
  // pointer to a room
  std::shared_ptr<crow::map_room> current_room;
  std::vector<glm::vec2> path;

  behavior_tree b_tree;

  void update_position(crow::entities objects, crow::entity enemy_index) {
    position = glm::vec2(objects.transforms_data[enemy_index][3][0],
                         objects.transforms_data[enemy_index][3][2]);
  }

  void update_target_position(crow::entities objects,
                              crow::entity target_index) {
    current_target_position =
        glm::vec2(objects.transforms_data[target_index][3][0],
                  objects.transforms_data[target_index][3][2]);
  }

  void set_current_room(std::shared_ptr<crow::map_room> room) {
    current_room = room;
  }

  void load_entity_data(crow::entities& objects, crow::entity enemy_index,
                        crow::entity target_index) {
    velocity = &objects.velocities[enemy_index];
    update_position(objects, enemy_index);
    target_pos = glm::vec2{0, 0};
    update_target_position(objects, target_index);
  }
  // child of my root node
  behavior_tree::sequence_node seq1;
  // 1st child of my sequence node
  behavior_tree::selector_node sel1;
  // 1st child of selector 1
  has_target ht1/*(adquired_target)*/;
  // 2nd child of my selector 1
  get_target gt1/*(adquired_target, &target_pos, current_target_position, path)*/;
  // 2nd child of sequence node
  behavior_tree::selector_node sel2;
  // 1st child of 2nd selector node
  has_path hp1/*(path)*/;
  // 2nd child of 2nd selector node
  ai_get_path gp1;
  has_target_changed htc1;
  move_to_target mtt1;
  behavior_tree::sequence_node seq2;

  void create_behavior_tree() {
    ht1.adquired_target = &adquired_target;

    gt1.adquired_target = &adquired_target;
    gt1.target = &target_pos;
    gt1.current_target_pos = &current_target_position;
    gt1.current_path = &path;

    hp1.current_path = &path;

    sel1.add_child(&ht1);
    sel1.add_child(&gt1);

    seq1.add_child(&sel1);

    gp1.adquired_target = &adquired_target;
    gp1.position = &position;
    gp1.target_position = &target_pos;
    gp1.current_room = current_room;
    gp1.path_container = &path;

    sel2.add_child(&hp1);
    sel2.add_child(&gp1);

    seq1.add_child(&sel2);

    htc1.current_target_position = &current_target_position;
    htc1.target_check = &adquired_target;
    htc1.target_position = &target_pos;

    mtt1.current_path = &path;
    mtt1.position = &position;
    mtt1.speed = 1.2f;
    mtt1.velocity = velocity;
    mtt1.adquired_target = &adquired_target;

    seq2.add_child(&htc1);
    seq2.add_child(&mtt1);

    seq1.add_child(&seq2);

    b_tree.set_root_child(&seq1);
  }

 private:
};