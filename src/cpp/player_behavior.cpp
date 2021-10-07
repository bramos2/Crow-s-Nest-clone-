#include "../hpp/player_behavior.hpp"

namespace crow {

auto get_angle(glm::vec2 const pos_from, glm::vec2 const pos_to) -> float {
  return std::atan2(pos_to.y - pos_from.y, pos_to.x - pos_from.x);
}

auto is_pathing(std::vector<glm::vec2> const curr_path) -> bool {
  if (curr_path.size() == 0) {
    return false;
  }
  return true;
}

void set_velocity(glm::vec2 destination, crow::entities& entity,
                  crow::entity ent, float speed) {
  // float worker_speed = 1.0f;
  float rad_angle = crow::get_angle(
      glm::vec2(entity.transforms_data[static_cast<size_t>(ent)][3][0],
                entity.transforms_data[static_cast<size_t>(ent)][3][2]),
      destination);
  float vel_y = std::sin(rad_angle) * speed;
  float vel_x = std::cos(rad_angle) * speed;
  entity.velocities[static_cast<size_t>(ent)] = glm::vec3{vel_x, 0, vel_y};
}

void set_velocity(glm::vec2 position, glm::vec2 destination,
                  glm::vec3& velocity, float speed) {
  float rad_angle = crow::get_angle(position, destination);
  float vel_y = std::sin(rad_angle) * speed;
  float vel_x = std::cos(rad_angle) * speed;
  velocity = glm::vec3{vel_x, 0, vel_y};
}

void path_through(player_behavior_data& p_data, crow::entities& entity,
                  crow::entity ent, float dt) {
  if (is_pathing(p_data.path_result)) {
    set_velocity(p_data.path_result.back(), entity, ent, p_data.worker_speed);
    glm::vec2 curr_pos =
        glm::vec2(entity.transforms_data[static_cast<size_t>(ent)][3][0],
                  entity.transforms_data[static_cast<size_t>(ent)][3][2]);
    glm::vec2 curr_vel =
        glm::vec2(entity.velocities[static_cast<size_t>(ent)].x * dt,
                  entity.velocities[static_cast<size_t>(ent)].z * dt);
    if (entity.velocities[static_cast<size_t>(ent)].x > 0) {
      if (curr_pos.x + curr_vel.x >= p_data.path_result.back().x) {
        p_data.path_result.pop_back();
      }
    }
    if (entity.velocities[static_cast<size_t>(ent)].x < 0) {
      if (curr_pos.x + curr_vel.x <= p_data.path_result.back().x) {
        p_data.path_result.pop_back();
      }
    }
  } else {
    entity.velocities[static_cast<size_t>(ent)] = glm::vec3(0.0f, 0.0f, 0.0f);
    if (p_data.interacting) {
      p_data.target->interact();
      p_data.interacting = false;
    }
  }
}

player_behavior_data::player_behavior_data() {}

player_behavior_data::~player_behavior_data() {}

}  // namespace crow
