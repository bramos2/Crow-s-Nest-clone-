#include "../hpp/camera.hpp"

#include <glm/gtx/vector_angle.hpp>

#include "../../debug_camera_control/debug_camera_control.hpp"

// #include <fmt/core.h>
#include <liblava/lava.hpp>

#include <glm/gtx/euler_angles.inl>

namespace crow {

auto get_floor_point(lava::camera& camera) -> glm::vec3 {
  glm::mat4 const view = camera.get_view();
  glm::vec3 const translation = camera.position;
  glm::vec3 const rotation = camera.rotation;  // X, Y, and Z angles in degrees.
  float height = translation.y;                // Assume the floor is at y == 0.

  glm::vec3 up = glm::vec3{0, 1, 0};
  // float pitch = -glm::acos(glm::dot(front, up));
  float pitch = rotation.x + 90;
  float theta = 90 - pitch;
  float forward_magnitude = translation.y / glm::sin(glm::radians(theta)) *
                            glm::sin(glm::radians(90.f));

  glm::vec3 front;
  front.x =
      -glm::cos(glm::radians(rotation.x)) * glm::sin(glm::radians(rotation.y));
  front.y = glm::sin(glm::radians(rotation.x));
  front.z =
      glm::cos(glm::radians(rotation.x)) * glm::cos(glm::radians(rotation.y));
  front = glm::normalize(front);

  auto result = front * forward_magnitude + translation;
  return result;
}

void initialize_debug_camera(lava::camera& camera) {
  camera.rotation_speed = 250;
  camera.movement_speed += 10;
  camera.position = lava::v3(0.0f, -4.036f, 8.304f);
  camera.rotation = lava::v3(0, 0, 0);
  // These globals are defined in debug_camera_control.hpp
  camera.set_movement_keys(debug_key_up, debug_key_down, debug_key_left,
                           debug_key_right);
}

}  // namespace crow
