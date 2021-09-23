#pragma once

#include <liblava/lava.hpp>

#include <vector>

#include "../hpp/behavior_tree.hpp"
#include "../hpp/map.hpp"
#include "../hpp/object.hpp"


// The behavior nodes used by the ai will be defined here
// for behavior will need a refence to the ai entity
// will need a reference to the active room for pathing

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

  void update_position(crow::entities objects, crow::entity enemy_index);

  void update_target_position(crow::entities objects,
                              crow::entity target_index);

  void set_current_room(std::shared_ptr<crow::map_room> room);

  void load_entity_data(crow::entities& objects, crow::entity enemy_index,
                        crow::entity target_index);

  void create_behavior_tree();

  void clean_tree();

};

struct has_target : public behavior_tree::node {
  virtual status run() override;
  has_target(bool& a_target); 
  bool* adquired_target;
};

struct get_target : public behavior_tree::node {
  virtual status run() override;

  get_target(bool& a_target, glm::vec2* t, glm::vec2& curr_t_pos,
             std::vector<glm::vec2>& path);

  bool* adquired_target;
  glm::vec2* target;
  glm::vec2* current_target_pos;
  std::vector<glm::vec2>* current_path;
};

struct has_path : public behavior_tree::node {  // node that checks if an
                                                // exsisting path exist
  virtual status run() override;

  has_path(std::vector<glm::vec2>& path);
  std::vector<glm::vec2>* current_path;
};

struct ai_get_path : public behavior_tree::node {
  virtual status run() override;

  /* ai_get_path(glm::vec2& pos, glm::vec2& target_pos, std::vector<glm::vec2>*
   * path) {}*/
  // will need a reference to the current room
  std::shared_ptr<crow::map_room> current_room;
  // a starting position
  glm::vec2* position;
  // a target destination
  glm::vec2* target_position;
  // path container
  std::vector<glm::vec2>* path_container;
  bool* adquired_target;
};

// name implies changed = passed, use an inverter decorator node after changes
struct has_target_changed : public behavior_tree::node {
  virtual status run() override;
  bool* target_check;
  glm::vec2* target_position;
  glm::vec2* current_target_position;
};

struct move_to_target : public behavior_tree::node {
  virtual status run() override;
  float speed;
  std::vector<glm::vec2>* current_path;
  glm::vec3* velocity;
  glm::vec2* position;
  bool* adquired_target;
};