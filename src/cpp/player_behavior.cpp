#include "../hpp/player_behavior.hpp"

namespace crow {
auto reached_destination(float2e const velocity, float2e const position,
                         float2e const destination, float const precicion) -> bool {
  if (position == destination) {
    return true;
  }

  bool result = false;

  const float2e diff1 = destination - position;
  const float l = diff1.length();

  const float2e diff2 = destination - (position + velocity);
  const float l2 = diff2.length();

  if (l2 > l) {
    result = true;
  } else {
    if (l < precicion) {
      result = true;
    }
  }

  /*const float2e temp = position + velocity;

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
auto get_angle(float2e const pos_from, float2e const pos_to) -> float {
  return std::atan2f(pos_to.y - pos_from.y, pos_to.x - pos_from.x);
}

auto is_pathing(std::vector<float2e> const curr_path) -> bool {
  if (curr_path.size() == 0) {
    return false;
  }
  return true;
}

void set_velocity(float2e destination, crow::entities& entity, size_t index,
                  float speed) {
    float4x4_a position = (float4x4_a&)entity.world_matrix[index];

    // float worker_speed = 1.0f;
    float rad_angle =
        crow::get_angle(float2e(position[3][0],
                                position[3][2]), destination);
    float vel_y = std::sinf(rad_angle) * speed;
    float vel_x = std::cosf(rad_angle) * speed;
    entity.velocities[index] = DirectX::XMFLOAT3{vel_x, 0, vel_y};
}

void set_velocity(float2e position, float2e destination,
                  float3e& velocity, float speed) {
  float rad_angle = crow::get_angle(position, destination);
  float vel_y = std::sinf(rad_angle) * speed;
  float vel_x = std::cosf(rad_angle) * speed;
  velocity = float3e{vel_x, 0, vel_y};
}

void path_through(player_behavior_data& p_data, crow::entities& entities,
                  size_t index, double const dt) {
  if (is_pathing(p_data.path_result)) {
    set_velocity(p_data.path_result.back(), entities, index,
                 p_data.worker_speed);
    float3e curr_pos = entities.get_world_position(index);

    float2e curr_vel = float2e(dt * entities.velocities[index].x,
                                   dt * entities.velocities[index].z);

    if (reached_destination(curr_vel, float2e(curr_pos.x, curr_pos.z),
                            p_data.path_result.back(), 0.5f)) {
      p_data.path_result.pop_back();
    }

    if (entities.mesh_ptrs[index]->animator && !entities.mesh_ptrs[index]->animator->is_running) {
        entities.mesh_ptrs[index]->animator->switch_animation(animator::anim_type::MOVING);
        entities.mesh_ptrs[index]->animator->is_running = true;
        entities.mesh_ptrs[index]->animator->is_acting = false;
    }

  } else {
    entities.velocities[index] = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
    if (p_data.interacting) {
      p_data.target->interact(index, entities);

      p_data.target = nullptr;
      p_data.interacting = false;
      // TODO ADD INTERACTING ANIMATIONS
    }
    if (entities.mesh_ptrs[index]->animator && entities.mesh_ptrs[index]->animator->is_running) {
        entities.mesh_ptrs[index]->animator->switch_animation(animator::anim_type::IDLE);
        entities.mesh_ptrs[index]->animator->is_running = false;
        entities.mesh_ptrs[index]->animator->is_acting = false;
    }
  }
}

player_behavior_data::player_behavior_data() {}

player_behavior_data::~player_behavior_data() {}

}  // namespace crow
