#pragma once
#include <liblava/lava.hpp>

#include <vector>

#include "../hpp/entities.hpp"
#include "../hpp/interactible.hpp"

namespace crow {

struct player_behavior_data {
  const float worker_walk_speed = 2.0f;
  const float worker_run_speed = 3.5f;
  std::vector<glm::vec2> path_result;
  float worker_speed = 0.f;
  bool interacting = false;
  // unsigned int current_room = 0;
  interactible* target = nullptr;
  player_behavior_data();
  ~player_behavior_data();
};

auto reached_destination(glm::vec2 const velocity, glm::vec2 const position,
                         glm::vec2 const destination) -> bool;

auto get_angle(glm::vec2 const pos_from, glm::vec2 const pos_to) -> float;

auto is_pathing(std::vector<glm::vec2> const curr_path) -> bool;

void set_velocity(glm::vec2 destination, crow::entities& entities, size_t index,
                  float speed);

void set_velocity(glm::vec2 position, glm::vec2 destination,
                  glm::vec3& velocity, float speed);

void path_through(player_behavior_data& p_data, crow::entities& entity,
                  size_t index, float const dt);

}  // namespace crow