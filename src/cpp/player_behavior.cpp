#include "../hpp/player_behavior.hpp"

namespace crow {
auto reached_destination(glm::vec2 const velocity, glm::vec2 const position,
                         glm::vec2 const destination) -> bool {
  if (position == destination) {
    return true;
  }

  bool result = false;

  const glm::vec2 diff1 = destination - position;
  const float l = glm::length(diff1);

  const glm::vec2 diff2 = destination - (position + velocity);
  const float l2 = glm::length(diff2);

  if (l2 > l) {
    result = true;
  } else {
    if (l < 0.2f) {
      result = true;
    }
  }

  /*const glm::vec2 temp = position + velocity;

  if (velocity.x != 0.f) {
    if (velocity.x > 0.f && temp.x >= destination.x) {
      result = true;
    } else if (velocity.x < 0.f && temp.x <= destination.x) {
      result = true;
    }
  } else if (velocity.y != 0.f) {
    if (velocity.y > 0 && temp.y >= destination.y) {
      result = true;
    } else if (velocity.y < 0.f && temp.y <= destination.y) {
      result = true;
    }
  }*/

  /* if (velocity.x == 0) {
     xc = true;
   } else if (velocity.x > 0 && temp.x >= destination.x) {
     xc = true;
   } else if (velocity.x < 0 && temp.x <= destination.x) {
     xc = true;
   }

   if (velocity.y == 0) {
     yc = true;
   } else if (velocity.y > 0 && temp.y >= destination.y) {
     yc = true;
   } else if (velocity.y < 0 && temp.y <= destination.y) {
     yc = true;
   }*/

  // result = (xc && yc);

  return result;
  /*(curr_vel.x > 0 &&
          curr_pos.x + curr_vel.x >= p_data.path_result.back().x);*/
}
auto get_angle(glm::vec2 const pos_from, glm::vec2 const pos_to) -> float {
  return std::atan2f(pos_to.y - pos_from.y, pos_to.x - pos_from.x);
}

auto is_pathing(std::vector<glm::vec2> const curr_path) -> bool {
  if (curr_path.size() == 0) {
    return false;
  }
  return true;
}

void set_velocity(glm::vec2 destination, crow::entities& entity, size_t index,
                  float speed) {
  // float worker_speed = 1.0f;
  float rad_angle =
      crow::get_angle(glm::vec2(entity.transforms_data[index][3][0],
                                entity.transforms_data[index][3][2]),
                      destination);
  float vel_y = std::sinf(rad_angle) * speed;
  float vel_x = std::cosf(rad_angle) * speed;
  entity.velocities[index] = glm::vec3{vel_x, 0, vel_y};
}

void set_velocity(glm::vec2 position, glm::vec2 destination,
                  glm::vec3& velocity, float speed) {
  float rad_angle = crow::get_angle(position, destination);
  float vel_y = std::sin(rad_angle) * speed;
  float vel_x = std::cos(rad_angle) * speed;
  velocity = glm::vec3{vel_x, 0, vel_y};
}

void path_through(player_behavior_data& p_data, crow::entities& entities,
                  size_t index, float const dt) {
  if (is_pathing(p_data.path_result)) {
    set_velocity(p_data.path_result.back(), entities, index,
                 p_data.worker_speed);
    glm::vec3 curr_pos = entities.get_world_position(index);

    glm::vec2 curr_vel = glm::vec2(dt * entities.velocities[index].x,
                                   dt * entities.velocities[index].z);

    if (reached_destination(curr_vel, glm::vec2(curr_pos.x, curr_pos.z),
                            p_data.path_result.back())) {
      p_data.path_result.pop_back();
    }

  } else {
    entities.velocities[index] = glm::vec3(0.0f, 0.0f, 0.0f);
    if (p_data.interacting) {
      p_data.target->interact(index, entities);

      p_data.target = nullptr;
      p_data.interacting = false;
    }
  }
}

player_behavior_data::player_behavior_data() {}

player_behavior_data::~player_behavior_data() {}

}  // namespace crow
