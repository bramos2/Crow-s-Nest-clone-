#pragma once

#include <liblava/lava.hpp>

#include <vector>

//#include "../hpp/behavior_tree.hpp"
#include "../hpp/entities.hpp"
#include "../hpp/interactible.hpp"
#include "../hpp/map.hpp"

namespace crow {

enum class status { FAILED = 0, PASSED, RUNNING };

// this class is meant to keep track of the ai by holding references to data the
// behavior tree will use
class ai_manager {
  // friend class crow::behavior_tree::leaf_node;

 public:
  // crow::behavior_tree btree;
  crow::entities *entities = nullptr;
  crow::level *curr_level = nullptr;
  crow::room *curr_room = nullptr;
  // the current path finding result
  std::vector<glm::vec2> path;
  // used when player is not the target, will increase over time
  float roam_speed = 2.f;
  // the speed at which the AI pursues the player
  const float run_speed = 8.f;
  // The current interactible the ai is chasing
  crow::interactible *target = nullptr;
  // this will be used for the AI's decision making process.
  crow::interactible *prev_target = nullptr;
  // This variable is used to determine when the AI will break a console or door
  unsigned int counter = 5;
  // used for cleanup purposes later on, will store ALL nodes
  //  std::vector<crow::behavior_tree::node*> nodes;
  bool interacting = false;
  bool checking_room = false;
  bool is_roaming = true;
  // used to check for how long we have roammed
  float roam_timer = 0.f;
  // the total amount of time we will be roaming
  float roam_total = 10.f;

  // init/creation methods
  // TODO: all methods to initialize and prepare the manager and behavior tree
  // go here, these need to be called prior to calling the run method
  void room_check();

  void init_manager(crow::entities *ent, crow::level *level);

  // behavior methods
  // TODO: all behavior methods must return a status to match the node function
  // pointer and take no parameters all the data needed is stored in the class
  // thus all methods have acess to what they need
  // private:
  // targetting branch behaviors

  //// check to know if we currently have a target
  // bool adquired_target = false;
  //// the velocity of the ai entity
  // glm::vec3* velocity;
  //// the position of the ai entity
  // glm::vec2 position;
  //// the target position currently being used for path finding, could be a
  /// door, / the player or a noise signal
  // glm::vec2 target_pos;
  //// used to consider if the ai needs to do a new path find
  // glm::vec2 current_target_position;
  //// pointer to a room
  // std::shared_ptr<crow::room> current_room;
  // std::vector<glm::vec2> path;

  // crow::interactible* target;

  // behavior_tree b_tree;

  // behavior_tree::sequence_node* seq1;
  // behavior_tree::selector_node* sel1;
  // has_target* ht1;
  // scan_room* sc1;
  // behavior_tree::selector_node* sel2;
  // has_path* hp1;
  // ai_get_path* gp1;
  // move_to_target* mtt1;

  //// has_target_changed* htc1 = new has_target_changed();

  // void update_position(crow::entities objects);

  // void update_target_position(crow::entities objects,
  //                            crow::entity target_index);

  // void set_current_room(std::shared_ptr<crow::room> room);

  // void load_entity_data(crow::entities& objects, crow::entity enemy_index,
  //                      crow::entity target_index);

  // void create_behavior_tree();

  // void clean_tree();
};

// roam branch
status roam_check(float dt, crow::ai_manager &m);
status roam_path(float dt, crow::ai_manager &m);

// target branch
status has_target(float dt, crow::ai_manager &m);
status target_player(float dt, crow::ai_manager &m);
status target_console(float dt, crow::ai_manager &m);
status target_door(float dt, crow::ai_manager &m);
status target_floor(float dt, crow::ai_manager &m);  // new

// path branch
status has_path(float dt, crow::ai_manager &m);
status is_path_currernt(float dt, crow::ai_manager &m);
status get_path(float dt, crow::ai_manager &m);

// move branch
status reached_target(float dt, crow::ai_manager &m);
status move(float dt, crow::ai_manager &m);

// interaction branch
status check_room(float dt, crow::ai_manager &m);  // new
status is_target_door(float dt, crow::ai_manager &m);
status handle_door(float dt, crow::ai_manager &m);
status destroy_target(float dt, crow::ai_manager &m);

}  // namespace crow

// The behavior nodes used by the ai will be defined here
// for behavior will need a refence to the ai entity
// will need a reference to the active room for pathing

// updated
// struct has_target : public behavior_tree::node {
//  virtual status run() override;
//  has_target(crow::interactible* target);
//  crow::interactible* target_i = nullptr;
//};
//
//// to be replaced for now by scan room
// struct get_target : public behavior_tree::node {
//  virtual status run() override;
//
//  get_target(bool& a_target, glm::vec2* t, glm::vec2& curr_t_pos,
//             std::vector<glm::vec2>& path);
//
//  bool* adquired_target;
//  glm::vec2* target;
//  glm::vec2* current_target_pos;
//  std::vector<glm::vec2>* current_path;
//};
//
//// replacing get target
// struct scan_room : public behavior_tree::node {
//  virtual status run() override;
//
//  scan_room(std::shared_ptr<crow::room> room, crow::interactible*& target);
//  std::shared_ptr<crow::room> current_room;
//  crow::interactible* target_i = nullptr;
//};
//
// struct has_path : public behavior_tree::node {  // node that checks if an
//                                                // exsisting path exist
//  virtual status run() override;
//
//  has_path(std::vector<glm::vec2>& path);
//  std::vector<glm::vec2>* current_path;
//};
//
// struct ai_get_path : public behavior_tree::node {
//  virtual status run() override;
//
//  /* ai_get_path(glm::vec2& pos, glm::vec2& target_pos,
//  std::vector<glm::vec2>*
//   * path) {}*/
//  // will need a reference to the current room
//  std::shared_ptr<crow::room> current_room;
//
//  crow::interactible* target_i = nullptr;
//  // a starting position
//  glm::vec2* position = nullptr;
//  // a target destination
//  // glm::vec2* target_position = nullptr;
//  // path container
//  std::vector<glm::vec2>* path_container;
//  // bool* adquired_target;
//};
//
//// name implies changed = passed, use an inverter decorator node after changes
//// will not be needed for this run
// struct has_target_changed : public behavior_tree::node {
//  virtual status run() override;
//  bool* target_check;
//  glm::vec2* target_position;
//  glm::vec2* current_target_position;
//};
//
////
// struct move_to_target : public behavior_tree::node {
//  virtual status run() override;
//  float speed;
//  std::vector<glm::vec2>* current_path;
//  glm::vec3* velocity;
//  glm::vec2* position;
//  crow::interactible* target_i;
//};