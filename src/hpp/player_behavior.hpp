#pragma once
#include <liblava/lava.hpp>

#include <vector>

#include "../hpp/object.hpp"

namespace crow {
std::vector<glm::vec2> path_result;

float get_angle(glm::vec2 const pos_from, glm::vec2 const pos_to) {
  return std::atan2(pos_to.y - pos_from.y, pos_to.x - pos_from.x);
}

bool is_pathing() {
  if (path_result.size() == 0) {
    return false;
  }
  return true;
}

void set_velocity(glm::vec2 destination, crow::entities& entity,
                  crow::entity ent, float speed) {
  // float worker_speed = 1.0f;
  float rad_angle =
      crow::get_angle(glm::vec2(entity.transforms_data[ent][3][0],
                                entity.transforms_data[ent][3][2]),
                      destination);
  float vel_y = sin(rad_angle) * speed;
  float vel_x = cos(rad_angle) * speed;
  entity.velocities[ent] = glm::vec3{vel_x, 0, vel_y};
}

void path_through(crow::entities& entity, crow::entity ent, float speed, float dt) {
  if (is_pathing()) {
    set_velocity(path_result.back(), entity, ent, speed);
    glm::vec2 curr_pos = glm::vec2(entity.transforms_data[ent][3][0],
                                   entity.transforms_data[ent][3][2]);
    glm::vec2 curr_vel = glm::vec2(entity.velocities[ent].x * dt,
                                   entity.velocities[ent].z * dt);
    if (entity.velocities[ent].x > 0) {
      if (curr_pos.x + curr_vel.x >= path_result.back().x) {
        path_result.pop_back();
      }
    }
    if (entity.velocities[ent].x < 0) {
      if (curr_pos.x + curr_vel.x <= path_result.back().x) {
        path_result.pop_back();
      }
    }
  } else {
    entity.velocities[ent] = glm::vec3(0.0f, 0.0f, 0.0f);
  }
}

}  // namespace crow