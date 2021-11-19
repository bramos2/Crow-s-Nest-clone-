#pragma once


#include <vector>

//#include "../hpp/behavior_tree.hpp"
#include "../hpp/entities.hpp"
#include "../hpp/interactible.hpp"
#include "../hpp/map.hpp"
#include "../hpp/math_types.hpp"

namespace crow {

enum class status { FAILED = 0, PASSED, RUNNING };

// this class is meant to keep track of the ai by holding references to data the
// behavior tree will use
class ai_manager {
  // friend class crow::behavior_tree::leaf_node;

 public:
  // crow::behavior_tree btree;
  size_t index = crow::entity::SPHYNX;
  int inter_index = -1;
  std::vector<crow::interactible*>* live_units = nullptr;

  crow::entities *entities = nullptr;
  crow::level *curr_level = nullptr;
  crow::room *curr_room = nullptr;
  // the current path finding result
  std::vector<float2e> path;
  // used when player is not the target, will increase over time
  float roam_speed = 4.f;
  // the speed at which the AI pursues the player
  const float run_speed = 8.f;
  // The current interactible the ai is chasing
  crow::interactible *target = nullptr;
  // this will be used for the AI's decision making process.
  //crow::interactible *prev_target = nullptr;
  // This variable is used to determine when the AI will break a console or door
  unsigned int counter = 0;
  // used for cleanup purposes later on, will store ALL nodes
  //  std::vector<crow::behavior_tree::node*> nodes;
  bool interacting = false;
  bool checking_room = false;
  bool is_roaming = true;

  // this bool will be used to prevent other AI from breaking doors
  bool friendly = false;

  // used to check for how long we have roammed
  float roam_timer = 0.f;
  // the total amount of time we will be roaming
  float roam_total = 2.8f;
  bool debug_mode = false;

  // init/creation methods
  // TODO: all methods to initialize and prepare the manager and behavior tree
  // go here, these need to be called prior to calling the run method
  void room_check();

  // initializes the ai manager with ecential data from the level and entities
  void init_manager(crow::entities *ent, crow::level *level, std::vector<crow::interactible*>* live_units);

  // debug function to print out the behavior name and the result status
  void print_status(std::string behavior, status b_status);
};

// npc behaviors
status passive_roam_check(float dt, crow::ai_manager& m); // does a roam check ignoright the player
status passive_has_target(float dt, crow::ai_manager& m); // checks for targetting while ignoring the player

// roam branch
status roam_check(float dt, crow::ai_manager &m); // checks roam time or if the player is in the room
status roam_path(float dt, crow::ai_manager &m); // gets a random path inside the current room

// target branch
status has_target(float dt, crow::ai_manager &m); // check if there is a current target and if it is valid, prioritizes player
status target_player(float dt, crow::ai_manager &m); // sets the target to the player
status target_console(float dt, crow::ai_manager &m); // sets the target to a console
status target_door(float dt, crow::ai_manager &m); // sets the target to a door

// path branch
status has_path(float dt, crow::ai_manager &m); // checks if there is a path
status is_path_currernt(float dt, crow::ai_manager &m); // checks if the path is current to our target
status get_path(float dt, crow::ai_manager &m);  // makes a new path towards our target

// move branch
status reached_target(float dt, crow::ai_manager &m); // checks if the path has been completed and we reched the target
status move(float dt, crow::ai_manager &m); // moves the AI based on current path

// interaction branch
status is_target_door(float dt, crow::ai_manager &m); // checks if target is a door
status handle_door(float dt, crow::ai_manager &m); // logic for using or attacking a door
status destroy_target(float dt, crow::ai_manager &m); // destroys the target

}  // namespace crow