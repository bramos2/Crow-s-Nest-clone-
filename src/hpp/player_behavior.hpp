#pragma once
#include <vector>

#include "../hpp/entities.hpp"
#include "../hpp/interactible.hpp"
#include "../hpp/super_matrix.hpp"

namespace crow {

struct player_behavior_data {
  const float worker_walk_speed = 2.0f;
  const float worker_run_speed = 3.5f;
  super_matrix p_matrix;
  std::vector<float2e> path_result;
  float worker_speed = 0.f;
  bool interacting = false;
  // unsigned int current_room = 0;
  interactible* target = nullptr;
  player_interact player_interact;
  player_behavior_data();
  ~player_behavior_data();
};

auto reached_destination(float2e const velocity, float2e const position,
                         float2e const destination) -> bool;

auto get_angle(float2e const pos_from, float2e const pos_to) -> float;

auto is_pathing(std::vector<float2e> const curr_path) -> bool;

void set_velocity(float2e destination, crow::entities& entities, size_t index,
                  float speed);

void set_velocity(float2e position, float2e destination,
                  float3e& velocity, float speed);

void path_through(player_behavior_data& p_data, crow::entities& entity,
                  size_t index, double const dt);

}  // namespace crow