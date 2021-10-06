#pragma once
#include <liblava/lava.hpp>

#include <vector>

#include "../hpp/interactible.hpp"
#include "../hpp/entities.hpp"

namespace crow {

struct player_behavior_data {
  const float worker_walk_speed = 2.0f;
  const float worker_run_speed = 3.5f;
  std::vector<glm::vec2> path_result;
  float worker_speed = 0.f;
  bool interacting = false;
  player_behavior_data();
 // interactible* target = nullptr;
 // player_interact* p_interact = new player_interact;
  /*item player_inventory;
  item* item_target = nullptr;*/
  //sd_console* sdc_target = nullptr;
  ~player_behavior_data();
};

auto get_angle(glm::vec2 const pos_from, glm::vec2 const pos_to) -> float;

auto is_pathing(std::vector<glm::vec2> const curr_path) -> bool;

void set_velocity(glm::vec2 destination, crow::entities2& entity,
                  crow::entity2 ent, float speed);

void set_velocity(glm::vec2 position, glm::vec2 destination,
                  glm::vec3& velocity, float speed);

void path_through(player_behavior_data& p_data, crow::entities2& entity,
                  crow::entity2 ent, float dt);

}  // namespace crow